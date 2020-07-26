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
     *@brief 获取当前相机上传的网速度
     */
    RESTULT_DATA_STRUCT *get_camera_net_speed(void);
    
    /**
     *@brief 查看项目是否上传完毕或者说上传过
     *@param  filePath        需要检查的场景的名称
     */
    RESTULT_DATA_STRUCT *check_camera_file_info(char *filePath);
    
    /**
     *@brief 重置相机镜头
     */
    RESTULT_DATA_STRUCT *reset_camera_lens_index(void);
    
    /**
     *@brief 获取相机当前镜头
     */
    RESTULT_DATA_STRUCT *get_camera_lens_index(void);
    
    /**
     *@brief 电量信息
     */
    RESTULT_DATA_STRUCT *get_camera_power_info(void);
    
    /**
     *@brief 获取相机的IMU
     */
    
    RESTULT_DATA_STRUCT *get_camera_imu(void);
    
    /**
     *@brief 获取相机的IMU
     */
    
    RESTULT_DATA_STRUCT *reboot_camera_imu(void);
    /**
     *@brief 格式化相机内存卡
     */
    
    RESTULT_DATA_STRUCT *send_format_sd(void);
    
    
    RESTULT_DATA_STRUCT *send_hearBreak_time(void);
    
    /**
     *@brief  发送CMD_KEEPALIVE 检查对方的socket服务是否正常
     */
    RESTULT_DATA_STRUCT * camera_socket_check(void);

    /**
     *@brief  边录边下视频
     *@param  sourcePath   相机中存储视频文件的路径
     *@param  savePath        手机本地保存视频文件地址的路径
     *@param  index               当前已经下载到手机本地的文件l的大小
     *@param  destLen           当前目标文件包的长度
     */
    bool camera_begin_readvideo(char *sourcePath,char *savePath, int index, int destLen);
    
    /**
     同步mp4头
     */
    bool camera_begin_syncvideohead(char *sourcePath,char *savePath, int destLen);
       
    
    /**
     *@brief 开始录制视频
     */
    
    RESTULT_DATA_STRUCT *camera_begin_video(char *path,char *videoName);
    
    /**
     *@brief 结束录制视频
     */
    RESTULT_DATA_STRUCT *camera_end_video(void);
    
    /**
     *@brief  设置分辨率
     *@param  hdFlag  1:低分辨率  0：搞清晰度
     */
    RESTULT_DATA_STRUCT * camera_set_lowHD(int hdFlag);
    
    /**
     *@brief 发送关机命令
     */
    
    RESTULT_DATA_STRUCT * camera_send_shutdown(void);
    
    
    /**
     *@brief 相机打包目录并且上传
     *@param filePath    打包项目名称
     */
    RESTULT_DATA_STRUCT * camera_send_dir_packet(char *filePath);
    
    /**
     *@brief 获取相机当前打包上传的进度信息
     */
    
    RESTULT_DATA_STRUCT * get_camera_upload_infomation(void);
    
    /**
     *@brief              拍摄照片读单张图片存储到本地
     *@imageType                       0：第一张图片 1:第二张图片  2：HDR模式才有 
     *@param localPath         图片存储的路径
     **/
    RESTULT_DATA_STRUCT * capture_single_recvImage(char *localPath,int imageType);
    
    /**
     *@brief                下载图片
     *@param desfilePath    图片存储的路径
     *@param filePath         下载图片的路径
     **/
    RESTULT_DATA_STRUCT  *take_file_downImage(char *desfilePath,char *filePath);
    
    /**
     *@brief              删除某张图片
     *@param filePath     需要删除图片的路径
     **/
    RESTULT_DATA_STRUCT  *take_file_deleteImage(char *filePath);
    /**
     *@brief              删除相机所有图片
     **/
    RESTULT_DATA_STRUCT  *take_file_deleteImageAll(void);
    
    
    /**
     *@brief 上传文件给相机
     **/
    RESTULT_DATA_STRUCT *upload_file(char *filePath, char *desFilePath);
    
    
    
    /**
     *@brief 删除项目
     *@filePath filePath 传项目文件路径
     **/
    RESTULT_DATA_STRUCT* delete_camera_project(char *filePath);
    
    
    /**
     *@brief 设置时区
     *@param timeZone  TIME_ZONE_E类型
     **/
    RESTULT_DATA_STRUCT* setCamera_time_zone(TIME_ZONE_E timeZone);
    
    /**
     *@brief 设置系统时间
     *@param sysTime  时间戳
     **/
    RESTULT_DATA_STRUCT* setCamera_sysTime(char * sysTime);
    
    /**
     *@brief 获取所有项目信息
     **/
    RESTULT_DATA_STRUCT* getCamera_project_allInfo(void);
    
    /**
     *@brief 获取wifi连接状态 STA连接状态 0:没有连接1:连接成功
     **/
    
    RESTULT_DATA_STRUCT* getCamera_wifi_status(void);
    
    
    /**
     *@brief 获取相机周围的AP热点 因为是耗时操作，所以新建socket连接，获取成功，销毁释放资源 使用5005，或者 5006 端口
     **/
    
    RESTULT_DATA_STRUCT * getCamera_SCANN_AP_list(void);
    
    /**1
     *@brief 获取相机 AP模式的Wi-Fi信息
     **/
    
    RESTULT_DATA_STRUCT *getCamera_wifi_AP(void);
    
    
    /**
     *@brief 设置相机本身AP的 Wi-Fi 的ssid和密码
     传ssid和passwd两个字段给我，ssid是wifi名称，暂不支持中文、空格等，密码有效长度8到16个字节，小于8个字节时，认为是不设密码
     *@param ssid        Wi-Fi路由器热点账号 value(char *)
     *@param password    Wi-Fi路由器热点密码 value(char *)
     **/
    RESTULT_DATA_STRUCT *setCamera_wifi_AP(char* ssid ,char* password);
    
    
    /**3
     *@brief 设置相机 STA模式的Wi-Fi信息z
     **/
    
    RESTULT_DATA_STRUCT* getCamera_wifi_info(void);
    
    
    /**4 已弃用
     *@brief 设置相机      设置相机去连接指定的wifi (已经)
     *@param wifiEnable  wifi设置 1连接wifi，0断开wifi value({0.1})
     *@param dhcpEnable  是否动态连接,0动态获取，1手动设置 动态获取IP，后面设置的 ipAddr 无效，相机从wifi路由器上动态获取IP地址 value({0.1})
     *@param ssid        Wi-Fi路由器热点账号 value(char *)
     *@param password    Wi-Fi路由器热点密码 value(char *)
     *@param status      保留，设置wifi信息固定为0。 当getCamera_wifi_info获取此字段对则用来判断项目是否连接路由
     *@param ipAddr      设备ip地址，非动态获取，手动设置有效   value(char *)
     *@param netMask     设备子网掩码,非动态获取，手动设置有效   value(char *)
     **/
    
    
    /*fixing**/
    RESTULT_DATA_STRUCT* setCamera_wifi_info(int wifiEnable,int dhcpEnable,int status,char* ssid,char* password,char* ipAddr,char* netMask);
    
    /**4
     *@brief 设置相机      设置相机去连接指定的wifi
     *@param wifiEnable  wifi设置 1连接wifi，0断开wifi value({0.1})
     *@param dhcpEnable  是否动态连接,1动态获取，0手动设置 动态获取IP，后面设置的 ipAddr 无效，相机从wifi路由器上动态获取IP地址 value({0.1})
     *@param ssid        Wi-Fi路由器热点账号 value(char *)
     *@param password    Wi-Fi路由器热点密码 value(char *)
     *@param mac         mac地址
     *@param AUTH        wifi验证方式（传1使用方式，请咨询unity版开发者）
     **/
    
    RESTULT_DATA_STRUCT *setCamera_link_wifi(char *ssid,char *password,char* mac,int AUTH,int wifiEnable,int dhcpEnable);
    
    
    /**
     *@brief 设置相机 图片上传的服务器地址的信息
     *@param serviceAdr 服务器地址
     **/
    RESTULT_DATA_STRUCT* setCamera_service_info(char *serviceAdr);
    
    
    /**
     *@brief 获取相机 图片上传的服务器地址的信息
     **/
    RESTULT_DATA_STRUCT* getCamera_service_info(void);
    
    /**
     *@brief 获取相机SD卡的信息
     **/
    RESTULT_DATA_STRUCT* getCamera_SD_info(void);
    
    
    /**
     *@brief 获取相机属性
     **/
    RESTULT_DATA_STRUCT* getType_AE(void);
    
    /**
     *@brief 设置相机属性   自动模式：设置Compensation参数，
     手动模式：设置ISO，ExpTime参数
     说明：任何模式下都需要传4个参数，无用参数请统一传值 0
     *@param Type         0自动 1手动      value ({0,1})
     *@param Compensation 亮度自动调整      value（0-100）float
     *@param ExpTime      手动模式下快门时间 value(1/100 1/80 1/66 1/50  1/40 1/33 1/20 1/12  1/8 1/5)
     *@param ISO          手动模式下感光度   value(100 200 400 800 1600 3200 6400 12800 25600 51200 102400 192000 384000 768000 819200)
     **/
    
    RESTULT_DATA_STRUCT* setType_AE(int Type, int Compensation, int ExpTime, int ISO);
    
    
    /**
    *@brief 设置相机属性   自动模式：设置Compensation参数，
    手动模式：设置ISO，ExpTime参数
    说明：任何模式下都需要传参数，        无用参数请统一传值 0
    *@param Type                                        0自动 1手动      value ({0,1})
    *@param Compensation                        亮度自动调整      value（0-100）float
    *@param ExpTime                                 手动模式下快门时间 value(1/100 1/80 1/66 1/50  1/40 1/33 1/20 1/12  1/8 1/5)
    *@param ISO                                         手动模式下感光度   value(100 200 400 800 1600 3200 6400 12800 25600 51200 102400 192000 384000 768000 819200)
    *@param ColorTempratureMode            0 :自动 1：手动
    *@param ColorTemprature                     2800-10000
    **/
    
    RESTULT_DATA_STRUCT* setType_AE_moreColorTemprature(int Type,
                                                        int Compensation,
                                                        int ExpTime,
                                                        int ISO,
                                                        int ColorTempratureMode,
                                                        int ColorTemprature);
    /*
     *@brief 设置自动测光
     */
    
    RESTULT_DATA_STRUCT *setAuto_Metering(void);
    
    
    /**
     *@brief 获取镜头的状态，
     */
    RESTULT_DATA_STRUCT *getEye_status(void);
    
    /**
     *@brief 获取相机状态信息getScannAP getAllProject getWifiStaStatus getWifiSta getDeviceInfo
     **/
    
    RESTULT_DATA_STRUCT* getDeviceInfo(void);
    
    /**
     *@brief 获取相机RTSP视频流地址。
     **/
    RESTULT_DATA_STRUCT* getRtspUrl(void);
    
    /**
     *@brief 拍照。 新增HDR参数
     *@param capture_type 拍照方式
      *CMD_CAPTURE_A,              ///< 拍摄A面（1，2镜头)
      *CMD_CAPTURE_B,              ///< 拍摄B面（3，4镜头）
      *CMD_CAPTURE_ALL,           ///< 拍摄4个镜头
     *@param HDR                          0不启用 1 启用 2自动
     **/
    //RESTULT_DATA_STRUCT* takeCapture_withFilePath(CMD_ID capture_type,char *filePath);
    RESTULT_DATA_STRUCT* takeCapture_withFilePath(CMD_ID capture_type,char *filePath,int HDR);
    
    /**
     *@brief 获取当前项目。
     **/
    
    RESTULT_DATA_STRUCT* getCamera_current_ProjectByName(void);
    
    
    /**
     *@brief 设置当前项目。
     *@param projectName 项目名称
     **/
    
    RESTULT_DATA_STRUCT* setCamera_current_ProjectByName(char *projectName);
    
    
    /**
     *@brief  判断传入文件或者文件夹是否存在 find me , I will happy you@
     *@param  filePath 传 “” 获取的是相机sd卡根目录所有文件
     **/
     RESTULT_DATA_STRUCT* getFileOrFloderIsExits(char *filePath);
       
    
    /**
     *@brief 获取文件列表
     *@param  filePath 传 “” 获取的是相机sd卡根目录所有文件
     **/
    
    RESTULT_DATA_STRUCT* getFileList(char *filePath);
    
    
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
