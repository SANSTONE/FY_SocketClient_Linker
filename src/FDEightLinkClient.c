/*lei.FY  
                         _0_
                       _oo0oo_
                      o8888888o
                      88" . "88
                      (| -_- |)
                      0\  =  /0
                    ___/`---'\___
                  .' \\|     |// '.
                 / \\|||  :  |||// \
                / _||||| -:- |||||- \
               |   | \\\  -  /// |   |
               | \_|  ''\---/''  |_/ |
               \  .-\__  '-'  ___/-. /
             ___'. .'  /--.--\  `. .'___
          ."" '<  `.___\_<|>_/___.' >' "".
         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
         \  \ `_.   \_ __\ /__ _/   .-` /  /
     =====`-.____`.___ \_____/___.-`___.-'=====
                       `=---='
*****************************************************
     ¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥¥
         €€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€
               $$$$$$$$$$$$$$$$$$$$$$$
                   BUDDHA_BLESS_YOU
                      AWAY_FROM
                         BUG
all eight eye carmera link API，*/

#include "FDEightLinkClient.h"
#include "ThirdParty/cJSON.h"
#include <pthread.h>
#include <math.h>
#include "ThirdParty/s2j.h"
#include <signal.h>
#include <time.h>
#include <sys/socket.h>

#define SENDBUFFSIZE 1024
#define FAILDTIPS    "send faild!"
#define SUCESSTIPS   "sucess"

fd_socket  fSocket =INVALID_SOCKET;    ///< default:INVALID_SOCET
bool       bConnected  = false;          ///< default: false
uint32_t   iConnectTimeout = 5000;              ///< default: 5000 ms     0:非阻塞
uint32_t   iRecvTimeout=15000;                 ///< default: 3000 ms     0:非阻塞
uint32_t   iRecvTimeout_long;


//传递参数结构体
typedef struct
{
    char      sendBuff[1024];
    COMHEAD   head;
    fd_socket fSocket;
}THREAD_PARAM;

RESTULT_DATA_STRUCT  *common_result;
//连接5000端口的线程
void *pthreadConnect (void *arg);

RESTULT_DATA_STRUCT * creat_cmd_socket(THREAD_PARAM param);
THREAD_PARAM creat_cmd_param(COMHEAD head);
pthread_t thread_cmd_recv;      //创建线程，发送命令
pthread_t thread_cmd_send;      //创建线程，接收命令
pthread_mutex_t timer_lock = PTHREAD_MUTEX_INITIALIZER;

//struct sockaddr_storage *remoteAddr;    ///< default: NULL

struct itimerval heart_time_value;
void uinit_timer(struct itimerval heart_timer);
void init_timer(struct itimerval heart_timer);

static void *jsonToPackinfo(cJSON* json_obj);


#define MAXTCPSIZE 614400

void prompt_info (int signo);

ssize_t disconnect_socket(fd_socket socket_handler,
                          struct sockaddr_storage *c_remoteAddr){
    fd_close_socket(&socket_handler);
    if (c_remoteAddr ->ss_family) {
        fd_free_sockaddr(&c_remoteAddr);
    }
    
    return 0;
}

typedef struct
{
    THREAD_PARAM  param;
    RESTULT_DATA_STRUCT *result;
}THREAD_CMD_PARAM;

static void *send_cmd_socket(void *arg);


void *send_cmd_socket(void *arg){
    THREAD_CMD_PARAM cmd_param = *(THREAD_CMD_PARAM *)arg;
    THREAD_PARAM param = cmd_param.param;
    RESTULT_DATA_STRUCT *result =cmd_param.result;
    
    fd_socket socket =  fd_creat_socket(SOCK_STREAM);
    if (socket ==INVALID_SOCKET) {
         return NULL;
    }
    //这里统一做端口分配c
    PORT_TYPE  current_iport;
    uint32_t   connect_timeOut;
    connect_timeOut = 10000;
    struct sockaddr_storage *remoteAddr =  fd_creat_remote_sockaddr(MAIN_IP_ADRR, current_iport);

    if (!fd_connect(socket, (struct sockaddr *)remoteAddr, connect_timeOut)) {
        disconnect_socket(socket,remoteAddr);
        return NULL;
    }
    ssize_t sendRet = fd_send(socket,param.sendBuff,sizeof(COMHEAD)+param.head.len);
    if (sendRet <1) {
        disconnect_socket(socket,remoteAddr);
        return NULL;
    }
   
    //printf("recv*****=sendBuff=%s------%d\n", param.sendBuff+8,sizeof(COMHEAD)+param.head.len);
    char ack[2*RECVBUFFSIZE];
    COMHEAD *rpy  = (COMHEAD *)ack;
    memset(ack, 0, sizeof(ack));
    
    //先接收前8个字节，是头cmd和数据长度信息
    ssize_t ret = fd_recv(socket, ack, 8, connect_timeOut);
    printf("recv1111*****=%s\n", ack);
    if (ret < 1) {
        disconnect_socket(socket,remoteAddr);
        return NULL;
    }
    if (rpy->len > 0) {
//         printf("recv*****=fdddd=%s\n", ack + 8);
        ssize_t  sret = recvData(ack+8, rpy->len, socket);
        if (sret >0) {
            printf("recv*****=%s\n", ack + 8);
            result->flag=1;
            strcpy(result->rdata, ack+8);
            result->data = SUCESSTIPS;
            result->len = rpy->len;
            result->cmdId = rpy->cmdId;
        }
    }
    disconnect_socket(socket,remoteAddr);
    return NULL;
    
}

RESTULT_DATA_STRUCT * creat_cmd_socket(THREAD_PARAM param){
    
    char  fdRecvBuff[2*RECVBUFFSIZE]; //2MB
    RESTULT_DATA_STRUCT  *result =(RESTULT_DATA_STRUCT *)fdRecvBuff;
    memset(fdRecvBuff,0,sizeof(fdRecvBuff));
    //如果 命令头接收成功，接收业务端字符串数据
    result->flag=0;
    result->data = FAILDTIPS;
    result->len = param.head.len;
    result->cmdId = param.head.cmdId;
    result->cSocket = 0;
    memset(result->rdata, 0, sizeof(result->rdata));
    
    THREAD_CMD_PARAM cmd_param;
    cmd_param.result = result;
    cmd_param.param = param;
    
    pthread_t cmd_thread;
    int thread_flag = pthread_create(&cmd_thread, 0,send_cmd_socket ,&cmd_param);
    if (thread_flag != 0) {
        printf("thread error:%s \n", strerror(errno));
        return result;
    }
    pthread_join(cmd_thread,NULL);
    return result;
}

void *take_downLoad_recvImage(void *arg);
//传入图片路径下载图片的接口
RESTULT_DATA_STRUCT  *take_file_downImage(char *desfilePath,char *filePath){
    
    char  fdRecvBuff[2*RECVBUFFSIZE]; //2MB
    RESTULT_DATA_STRUCT  *result =(RESTULT_DATA_STRUCT *)fdRecvBuff;
    memset(fdRecvBuff,0,sizeof(fdRecvBuff));
    //如果 命令头接收成功，接收业务端字符串数据
    result->flag=0;
    //result->data = malloc(sizeof(char));
    result->data = FAILDTIPS;
    result->len = 0;
    result->cmdId = 4146;
    result->cSocket = 0;
        
    DWONIMAGE_TPHREAD_PARAM  param;
    param.localFile =  desfilePath;
    param.filePath  =  filePath;
    param.iPort     =  PORT_TYPE_DOWNLOAD_IMG1;
    param.result    =  result;
    
    
    //创建线程去下载图片
    pthread_t down_pthread;
    ssize_t  retDown = pthread_create(&down_pthread, NULL,&take_downLoad_recvImage, &param);
    if (retDown<0) {
        return result;
    }
    
    pthread_join(down_pthread, NULL);
    return result;
}

void *take_downLoad_recvImage(void *arg){
    DWONIMAGE_TPHREAD_PARAM param = *((DWONIMAGE_TPHREAD_PARAM *)arg);
    RESTULT_DATA_STRUCT *result = param.result;
    
    //连接
    fd_socket  socket= fd_creat_socket(SOCK_STREAM);
    struct sockaddr_storage * down_remoteAddr = fd_creat_remote_sockaddr(MAIN_IP_ADRR, param.iPort);
    fd_connect(socket, (struct sockaddr *)down_remoteAddr, iConnectTimeout);
    
    if (socket != INVALID_SOCKET && down_remoteAddr) {
        printf("lei>>>>>scoket sucess \n");
    }else{
        disconnect_socket(socket,down_remoteAddr);
        return  NULL;
    }
    //发送命令
    //如果 命令头接收成功，接收业务端字符串数据
    result->flag=0;
    //result->data = malloc(sizeof(char));
    result->data = FAILDTIPS;
    result->len = 0;
    result->cmdId = 4146;
    result->cSocket = socket;
    
    
    char  sendBuff[1024];
    memset(sendBuff,0, sizeof(sendBuff));
    
    cJSON *json_root_node  = cJSON_CreateObject();
    cJSON_AddStringToObject(json_root_node,"filePath",param.filePath); //{{\"filePath\":\"projectName\"}}
    char  *send_json= cJSON_PrintUnformatted(json_root_node);
    strcpy(sendBuff+8,send_json);
    
    COMHEAD sendHead;
    sendHead.cmdId =4146;
    sendHead.len = (int)strlen(send_json);
    memcpy(sendBuff, &sendHead, 8); //老套路，前8个直接放头
    //加入json字符串 传入你要下载的图片的路径
    ssize_t  sFlag = fd_send(socket,sendBuff, sizeof(COMHEAD)+sendHead.len);
    
    if (sFlag < 1 ) {
        printf("下图指令发送失败");
        disconnect_socket(socket,down_remoteAddr);
        return NULL;
    }
    //如果成功，打开文件指针，开始接受图片二进制流
    FILE   *fp;
    //char   *fileName = ".jpg";
    fp = fopen(param.localFile, "wb+");
    char     recvk[1024] = {0};
    while (1) {
        ssize_t rhFlag = fd_recv(socket,recvk,sizeof(COMHEAD), iRecvTimeout);
        
        if (rhFlag <1) {
            break;
        }else{
            COMHEAD *rHead = (COMHEAD *)recvk;
            if (rHead->cmdId != 4146 || rHead->len < 1) {
                break;
            }
            ssize_t buffRecvFlag = fd_recv(socket,recvk+8,rHead->len,iRecvTimeout);
            if (buffRecvFlag <1) {
                break;
            }
            PACKINFO *pPackinfo;
            cJSON *sJsonCode;
            char *recvImageBuff;
            sJsonCode=cJSON_Parse((char *)&recvk[sizeof(COMHEAD)]);
            pPackinfo=jsonToPackinfo(sJsonCode);
            //接收到图片包的json描述信息
            if (pPackinfo->state!=1 || pPackinfo->curLen <0) {
                break;
            }
            recvImageBuff = (char *)malloc(pPackinfo->curLen);
            //接收图片文件流
            ssize_t recvBuffFlag = fd_recv(socket, recvImageBuff,pPackinfo->curLen, iRecvTimeout);
            if (recvBuffFlag <1 ) {
                free(recvImageBuff);
                break;
            }
            unsigned int dataCRC;
            dataCRC = getDataCRC(-1, (unsigned char *)recvImageBuff, pPackinfo->curLen);
            
            dataCRC=(unsigned int )dataCRC;
            dataCRC=abs((int)dataCRC);
            printf("my application dataCRC = %d \n recvImageBuff dataCRC = %d \n",dataCRC,pPackinfo->dataCRC);
            if (dataCRC != pPackinfo->dataCRC) {
                
                sendHead.cmdId= 4146;
                sendHead.len = 1;
                char  tSendBuff[1024];
                memset(tSendBuff,0, sizeof(tSendBuff));
                memcpy(tSendBuff, &sendHead, 8); //老套路，前8个直接放头
                
                ssize_t midFlag =fd_send(socket,tSendBuff, sizeof(COMHEAD)+sendHead.len);
                
                if(midFlag < 1)
                {
                    free(recvImageBuff);
                    break;
                }
            }
            else{
                //数据流校验正确，文件流写入
                fwrite(recvImageBuff, 1, pPackinfo->curLen, fp);
                free(recvImageBuff);
                
                sendHead.cmdId= 4146;
                sendHead.len = 0;
                
                //发送命令
                char  tSendBuff[1024];
                memset(tSendBuff,0, sizeof(tSendBuff));
                memcpy(tSendBuff, &sendHead, 8); //老套路，前8个直接放头
                
                ssize_t tailFlag =fd_send(socket,tSendBuff, sizeof(COMHEAD)+sendHead.len);
                
                if(tailFlag < 1)
                {
                    fclose(fp);
                    break;
                }
                if(pPackinfo->cntPack == pPackinfo->curPack+1)
                {
                    printf("下载成功*******\n");
                    result->flag=1;
                    //result->data = malloc(sizeof(char));
                    result->data = SUCESSTIPS;
                    result->len = 1;
                    result->cmdId = 4146;
                    result->cSocket = socket;
                    fflush(fp);
                    fclose(fp);
                    break;
                }
            }
        }
    }
    
    disconnect_socket(socket,down_remoteAddr);
    return NULL;
}

THREAD_PARAM creat_cmd_param(COMHEAD head ){
    THREAD_PARAM param;
    memcpy(param.sendBuff,&head,8);
    param.head = head;
    param.fSocket = fSocket;
    return param;
}


ssize_t  recvData(char *pData, size_t iDatalen, fd_socket socket_handler){
    
    return fd_recv(socket_handler, pData, iDatalen, iRecvTimeout);
}

ssize_t sendData(char *pData, size_t iDataLen ,fd_socket socket_handler ){
    
    return fd_send(socket_handler, pData, iDataLen);
}

static void *jsonToPackinfo(cJSON* json_obj)
{
    s2j_create_struct_obj(packinfo, PACKINFO);
    s2j_struct_get_basic_element(packinfo, json_obj, int, state);
    s2j_struct_get_basic_element(packinfo, json_obj, int, cntPack);
    s2j_struct_get_basic_element(packinfo, json_obj, int, curPack);
    s2j_struct_get_basic_element(packinfo, json_obj, int, curLen);
    s2j_struct_get_basic_element(packinfo, json_obj, int, dataCRC);
    return packinfo;
    
}


unsigned int getDataCRC(unsigned int  LastCRC, unsigned char *lpData, int Size)
{
    int i = 0;
    unsigned char *buffer = NULL;
    int len = 0;
    unsigned int  m_crc = LastCRC;
    unsigned int crc32_table[256];
    unsigned int ulPolynomial = 0x04C11DB7;
    
    for (i=0; i<=0xFF; i++)
    {
        unsigned int value = 0;
        int ref = i;
        int n = 0;
        int j = 0;
        
        for (n=1; n<(8+1); n++)
        {
            if (ref & 1)
            {
                value |= 1 << (8 - n);
            }
            ref >>= 1;
        }
        crc32_table[i]= value << 24;
        for (j=0; j<8; j++)
        {
            crc32_table[i] = (crc32_table[i] << 1) ^ (crc32_table[i] & (1 << 31) ? ulPolynomial : 0);
        }
        value = 0;
        ref = crc32_table[i];
        for (n=1; n<(32+1); n++)
        {
            if (ref & 1)
            {
                value |= 1 << (32 - n);
            }
            ref >>= 1;
        }
        crc32_table[i] = value;
    }
    //caculate the crc check sum
    // m_crc = -1;
    buffer =lpData;
    len = Size;
    while (len--)
    {
        //printf("crc_value *****%x  \n",(m_crc & 0xFF) ^ *buffer);
        m_crc = (m_crc >> 8) ^ crc32_table[(m_crc & 0xFF) ^ *buffer++];
    }
    return m_crc;//^ 0xffffffff;
}

static const uint32_t crc32tab[] = {
    0 , 1996959894 , -301047508 , -1727442502, 124634137  , 1886057615 ,
    -379345611 , -1637575261, 249268274  , 2044508324 , -522852066 , -1747789432,
    162941995  , 2125561021 , -407360249 , -1866523247, 498536548  , 1789927666 ,
    -205950648 , -2067906082, 450548861  , 1843258603 , -187386543 , -2083289657,
    325883990  , 1684777152 , -43845254  , -1973040660, 335633487  , 1661365465 ,
    -99664541  , -1928851979, 997073096  , 1281953886 , -715111964 , -1570279054,
    1006888145 , 1258607687 , -770865667 , -1526024853, 901097722  , 1119000684 ,
    -608450090 , -1396901568, 853044451  , 1172266101 , -589951537 , -1412350631,
    651767980  , 1373503546 , -925412992 , -1076862698, 565507253  , 1454621731 ,
    -809855591 , -1195530993, 671266974  , 1594198024 , -972236366 , -1324619484,
    795835527  , 1483230225 , -1050600021, -1234817731, 1994146192 , 31158534  ,
    -1731059524, -271249366 , 1907459465 , 112637215  , -1614814043, -390540237 ,
    2013776290 , 251722036  , -1777751922, -519137256 , 2137656763 , 141376813  ,
    -1855689577, -429695999 , 1802195444 , 476864866  , -2056965928, -228458418 ,
    1812370925 , 453092731  , -2113342271, -183516073 , 1706088902 , 314042704  ,
    -1950435094, -54949764  , 1658658271 , 366619977  , -1932296973, -69972891  ,
    1303535960 , 984961486  , -1547960204, -725929758 , 1256170817 , 1037604311 ,
    -1529756563, -740887301 , 1131014506 , 879679996  , -1385723834, -631195440 ,
    1141124467 , 855842277  , -1442165665, -586318647 , 1342533948 , 654459306  ,
    -1106571248, -921952122 , 1466479909 , 544179635  , -1184443383, -832445281 ,
    1591671054 , 702138776  , -1328506846, -942167884 , 1504918807 , 783551873  ,
    -1212326853, -1061524307, -306674912 , -1698712650, 62317068   , 1957810842 ,
    -355121351 , -1647151185, 81470997   , 1943803523 , -480048366 , -1805370492,
    225274430  , 2053790376 , -468791541 , -1828061283, 167816743  , 2097651377 ,
    -267414716 , -2029476910, 503444072  , 1762050814 , -144550051 , -2140837941,
    426522225  , 1852507879 , -19653770  , -1982649376, 282753626  , 1742555852 ,
    -105259153 , -1900089351, 397917763  , 1622183637 , -690576408 , -1580100738,
    953729732  , 1340076626 , -776247311 , -1497606297, 1068828381 , 1219638859 ,
    -670225446 , -1358292148, 906185462  , 1090812512 , -547295293 , -1469587627,
    829329135  , 1181335161 , -882789492 , -1134132454, 628085408  , 1382605366 ,
    -871598187 , -1156888829, 570562233  , 1426400815 , -977650754 , -1296233688,
    733239954  , 1555261956 , -1026031705, -1244606671, 752459403  , 1541320221 ,
    -1687895376, -328994266 , 1969922972 , 40735498   , -1677130071, -351390145 ,
    1913087877 , 83908371  , -1782625662, -491226604 , 2075208622 , 213261112  ,
    -1831694693, -438977011 , 2094854071 , 198958881  , -2032938284, -237706686 ,
    1759359992 , 534414190  , -2118248755, -155638181 , 1873836001 , 414664567  ,
    -2012718362, -15766928  , 1711684554 , 285281116  , -1889165569, -127750551 ,
    1634467795 , 376229701  , -1609899400, -686959890 , 1308918612 , 956543938  ,
    -1486412191, -799009033 , 1231636301 , 1047427035 , -1362007478, -640263460 ,
    1088359270 , 936918000  , -1447252397, -558129467 , 1202900863 , 817233897  ,
    -1111625188, -893730166 , 1404277552 , 615818150  , -1160759803, -841546093 ,
    1423857449 , 601450431  , -1285129682, -1000256840, 1567103746 , 711928724  ,
    -1274298825, -1022587231, 1510334235 , 755167117
};

uint32_t now_crc32(const unsigned char *buf, uint32_t size)
{
    uint32_t i, crc;
    crc = -1;
    for (i = 0; i < size; i++)
        crc = crc32tab[(crc ^ buf[i]) & 0xff] ^ (crc >> 8);
    return crc^0xFFFFFFFF;
}



