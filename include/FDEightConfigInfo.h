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

#ifndef FDEightConfigInfo_h
#define FDEightConfigInfo_h

#include <stdio.h>
#include "FDEightLinkSocket.h"
typedef enum EIGHTEYE_ERROR_TYPE
{
    ERROR_LINK= 0,///< 连接错误
    ERROR_PARAM   ///< 参数错误
}EIGHTEYE_ERROR;


typedef struct
{
    int  cmdId;
    int  len;
}COMHEAD;

typedef struct
{
    unsigned int state;
    unsigned int cntPack;
    unsigned int curPack;
    unsigned int curLen;
    unsigned int dataCRC;
    
}PACKINFO;

/**
 *@brief 端口使用规范枚举
 *普通控制指令              5000       keeplive
 *                      5005，5006  阅后即焚
 *                      5001，5002  阅后即焚
 **/
typedef enum _PORT_TYPE
{
    PORT_TYPE_CMD = 5000,           ///< 普通功能命令专用端口
    PORT_TYPE_DOWNLOAD_IMG1 = 5000,
    PORT_TYPE_DOWNLOAD_IMG2 = 5000,
    PORT_TYPE_WIFILIST = 5000,      ///< 
    PORT_TYPE_FILELIST = 5000       ///< 
}PORT_TYPE;


/**
 *@brief  返回值结构体
 **/
#define RECVBUFFSIZE 1024*256
typedef struct
{
    int        flag;
    int        len;
    int        cmdId;
    fd_socket  cSocket;
    char       *data;
    char       rdata[RECVBUFFSIZE];
}RESTULT_DATA_STRUCT; 



/*
 *读图线程函数传参结构体
 */
typedef struct
{
    char                    *localFile;
    PORT_TYPE               iPort;
    RESTULT_DATA_STRUCT     *result;
}RECVIMAGE_TPHREAD_PARAM;


/*
 *下载线程函数传参结构体
 */
typedef struct
{
    char                    *localFile; //下载图片本地存放的路径
    char                    *filePath;  //下载图片路径
    PORT_TYPE               iPort;
    RESTULT_DATA_STRUCT     *result;
}DWONIMAGE_TPHREAD_PARAM;


/*
 *获取参相机wifi列表参数
 */

typedef struct
{
    RESTULT_DATA_STRUCT     *result;
    
}GETAP_TPHERAD_PARAM;


#define MAIN_IP_ADRR  "192.168.10.1"

typedef struct
{
    
    int  index;
    char filePath[256];
    
}READ_VIDEO_DATA;

typedef struct
{
    
    int  cmdId;
    int  len;
    int  index;
    int  destLen;
    char filePath[256];
    
}READ_VIDEO_PACK;

#define SIZE_PRE_PACK_DATA 40960

typedef struct
{
    int  cur;
    int  total;
    int  dataLen;
    char data[SIZE_PRE_PACK_DATA];
}RTN_VIDEO_PACK;

#endif /* FDEightConfigInfo_h */
