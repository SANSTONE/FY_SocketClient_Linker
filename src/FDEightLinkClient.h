/*lei.FY  6.1 special Version:相机拍摄，新增 HDR设置参数
          7.1 special Version:相机镜头，设置和获取
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

#include <stdio.h>
#include "FDEightLinkSocket.h"
#include "FDEightConfigInfo.h"

#if defined(__cplusplus)
extern "C"
    {
#endif
    /**
     *@brief                下载图片
     *@param desfilePath    图片存储的路径
     *@param filePath         下载图片的路径
     **/
    RESTULT_DATA_STRUCT  *take_file_downImage(char *desfilePath,char *filePath);
    

    
    /**
     *@brief  连接socket
     *@param  pHost 连接IP
     *@param  iPort 连接端口
     *Return  连接成功，0：成功 -1 失败
     **/
    
    int  connect_socket_server(char *pHost, uint16_t iPort);
    
    /**
     *@brief getDataCRC
     *@param  LastCRC CRC校验
     *@param  lpData  CRC校验
     *@param  Size    CRC校验
     **/
    unsigned int getDataCRC(unsigned int  LastCRC,  unsigned char *lpData, int Size);
    
    /**
     *@brief recvData
     *@param  pData           收到数据
     *@param  iDatalen        数据长度
     *@param  socket_handler  socket句柄
     **/
    ssize_t  recvData(char *pData, size_t iDatalen,fd_socket socket_handler);
    
    /**
     *@brief 获取文件列表
     *@param  pData           发送数据
     *@param  iDataLen        数据长度
     *@param  socket_handler  socket句柄
     **/
    ssize_t sendData(char *pData, size_t iDataLen,fd_socket socket_handler);
    
    /**
     *@brief  获取文件列表
     *@param  socket_handler      socket句柄
     *@param  c_remoteAddr        连接字
     **/
    
    ssize_t disconnect_socket(fd_socket socket_handler,
                              struct sockaddr_storage *c_remoteAddr);
    
    
    
#if defined(__cplusplus)
    }
#endif
/*new updata 2019923*/
