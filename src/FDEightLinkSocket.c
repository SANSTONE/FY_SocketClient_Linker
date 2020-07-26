//
//  FDEightLinkSocket.c
//  EightEyeLink
//
//  Created by lei.FY on 2019/6/5.
//  Copyright © 2019 Lei.FY. All rights reserved.
//

#include "FDEightLinkSocket.h"

#if defined(__APPLE__)
#include "TargetConditionals.h"
#endif

//#if defined(__ANDROID__)
//
//#define SO_NOSIGPIPE MSG_NOSIGNAL
//
//extern int getifaddrs(struct ifaddrs** result);
//extern void freeifaddrs(struct ifaddrs* addrs);
//#endif

static void fd_log(const char *fmt,...){
#if DEBUG
    printf("[FDCSocket] ");
    va_list args;
    va_start (args, fmt);
    vprintf(fmt, args);
    va_end (args);
    printf("\n");
#endif
   
}

bool fd_is_ipv6addr(const char *pAddr){
    if (!pAddr) {
        return false;
    }
    struct addrinfo hints, *res0;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    
    int err = getaddrinfo(pAddr, "8000", &hints, &res0);
    if (err != 0) {
        fd_log("%s: %s", __func__, gai_strerror(err));
        return false;
    }
    
    bool ret = (res0->ai_addr->sa_family == AF_INET6) ? true : false;
    freeaddrinfo(res0);
    return ret;
}

bool fd_format_ipv4_address(struct in_addr *ipv4Addr, char *pAddr){
    if (!ipv4Addr || !pAddr) {
        return false;
    }
    memset(pAddr, 0, INET_ADDRSTRLEN);
    if(inet_ntop(AF_INET, ipv4Addr, pAddr, INET_ADDRSTRLEN) != NULL){
        return true;
    }
    
    return false;
}

bool fd_format_ipv6_address(struct in6_addr *ipv6Addr, char *pAddr){
    if (!ipv6Addr || !pAddr) {
        return false;
    }
    memset(pAddr, 0, INET6_ADDRSTRLEN);
    if(inet_ntop(AF_INET6, ipv6Addr, pAddr, INET6_ADDRSTRLEN) != NULL){
        return true;
    }
    
    return false;
}

bool fd_get_address_by_host(char *pHost, char *pAddr){
    if (!pHost || pAddr) {
        return false;
    }
    
    bool bret = false;
    struct addrinfo hints;
    struct addrinfo *res, *cur;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family     = AF_UNSPEC;//AF_UNSPEC;
    hints.ai_socktype   = SOCK_DGRAM;
    hints.ai_flags      = AI_DEFAULT;
    hints.ai_protocol   = IPPROTO_IP;   // IP协议
    
    int status = getaddrinfo(pHost, "8000", &hints, &res);
    if (status != 0) {
        fd_log("%s error: getaddrinfo failed[%s(%d)]", strerror(status), status);
        return false;
    }
    
    for (cur = res; cur; cur = cur->ai_next) {
        if (cur->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)cur->ai_addr;
            bret = fd_format_ipv4_address(&ipv4->sin_addr, pAddr);
        }
        else if (cur->ai_family == AF_INET6){
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)cur->ai_addr;
            bret = fd_format_ipv6_address(&ipv6->sin6_addr, pAddr);
        }
    }
    
    freeaddrinfo(res);
    
    return bret;
}

bool fd_get_local_ipaddr(char *pLAddr){
    bool bresult = false;
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    
    if (getifaddrs(&interfaces) == 0) {  // 0 表示获取成功
        temp_addr = interfaces;
        while (temp_addr != NULL) {

    /**
     * if ios ,when wifi alive ,get wifi ifa_name first ,or other find zhe  pdp_ip0
     * **/
#if defined(__ANDROID__)
            if (strcmp(temp_addr->ifa_name, "wlan0") == 0)         //"wlan0" wifi (Android)
#else
                if ((strcmp(temp_addr->ifa_name, "en0") == 0) ||       //"en0" wifi (iOS)
                    (strcmp(temp_addr->ifa_name, "en1") == 0))         //"en1" wifi & 网线 (macOS)))
#endif
                {
                    if (temp_addr->ifa_addr->sa_family == AF_INET) {
                        char ipv4Addr[INET6_ADDRSTRLEN] = {0};
                        struct sockaddr_in *ipv4 = (struct sockaddr_in *)temp_addr->ifa_addr;
                        if (fd_format_ipv4_address(&ipv4->sin_addr, ipv4Addr) &&
                            (strcmp(ipv4Addr, "") != 0)) {
                            memset(pLAddr, 0, INET6_ADDRSTRLEN);
                            memcpy(pLAddr, ipv4Addr, INET6_ADDRSTRLEN);
                            bresult = true;
                        }
                    }
                    else if (temp_addr->ifa_addr->sa_family == AF_INET6){
                        char ipv6Addr[INET6_ADDRSTRLEN] = {0};
                        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)temp_addr->ifa_addr;
                        if (fd_format_ipv6_address(&ipv6->sin6_addr, ipv6Addr) &&
                            (strcmp(ipv6Addr, "") != 0) &&
                            (strncasecmp(ipv6Addr, "fe80", strlen("fe80")) != 0)) {
                            memset(pLAddr, 0, INET6_ADDRSTRLEN);
                            memcpy(pLAddr, temp_addr->ifa_addr, INET6_ADDRSTRLEN);
                            bresult = true;
                            break;
                        }
                    }
                }else if (strcmp(temp_addr->ifa_name, "pdp_ip0") == 0){  //"pdp_ip0" 蜂窝网络 (iOS)
                    if (temp_addr->ifa_addr->sa_family == AF_INET) {
                        char ipv4Addr[INET6_ADDRSTRLEN] = {0};
                        struct sockaddr_in *ipv4 = (struct sockaddr_in *)temp_addr->ifa_addr;
                        if (fd_format_ipv4_address(&ipv4->sin_addr, ipv4Addr) &&
                            (strcmp(ipv4Addr, "") != 0)) {
                            memset(pLAddr, 0, INET6_ADDRSTRLEN);
                            memcpy(pLAddr, ipv4Addr, INET6_ADDRSTRLEN);
                            bresult = true;
                        }
                    }
                    else if (temp_addr->ifa_addr->sa_family == AF_INET6){
                        char ipv6Addr[INET6_ADDRSTRLEN] = {0};
                        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)temp_addr->ifa_addr;
                        if (fd_format_ipv6_address(&ipv6->sin6_addr, ipv6Addr) &&
                            (strcmp(ipv6Addr, "") != 0) &&
                            (strncasecmp(ipv6Addr, "fe80", strlen("fe80")) != 0)) {
                            memset(pLAddr, 0, INET6_ADDRSTRLEN);
                            memcpy(pLAddr, temp_addr->ifa_addr, INET6_ADDRSTRLEN);
                            bresult = true;
                            break;
                        }
                    }
                }
            
            temp_addr = temp_addr->ifa_next;
        }
    }
    
    freeifaddrs(interfaces);
    return bresult;
}

bool fd_get_local_addrInfo(struct sockaddr_storage *pLAddr){
    bool bret = false;
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    if (getifaddrs(&interfaces) != 0) {// 0 表示获取成功
        return bret;
    }
    
    temp_addr = interfaces;
    while (temp_addr != NULL) {
#if defined(__ANDROID__)
        if (strcmp(temp_addr->ifa_name, "wlan0") == 0)         //"wlan0" wifi (Android)
#else
            if ((strcmp(temp_addr->ifa_name, "en0") == 0) ||       //"en0" wifi (iOS)
                (strcmp(temp_addr->ifa_name, "en1") == 0))         //"en1" wifi & 网线 (macOS)))
#endif
            {
                if (temp_addr->ifa_addr->sa_family == AF_INET) {
                    char ipv4Addr[INET_ADDRSTRLEN] = {0};
                    struct sockaddr_in *ipv4 = (struct sockaddr_in *)temp_addr->ifa_addr;
                    if (fd_format_ipv4_address(&ipv4->sin_addr, ipv4Addr) &&
                        (strcmp(ipv4Addr, "") != 0)) {
                        memset(pLAddr, 0, sizeof(struct sockaddr_storage));
                        memcpy(pLAddr, temp_addr->ifa_addr, sizeof(struct sockaddr_storage));
                        bret = true;
                    }
                }
                else if (temp_addr->ifa_addr->sa_family == AF_INET6){
                    char ipv6Addr[INET6_ADDRSTRLEN] = {0};
                    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)temp_addr->ifa_addr;
                    if (fd_format_ipv6_address(&ipv6->sin6_addr, ipv6Addr) &&
                        (strcmp(ipv6Addr, "") != 0) &&
                        (strncasecmp(ipv6Addr, "fe80", strlen("fe80")) != 0)) {
                        memset(pLAddr, 0, sizeof(struct sockaddr_storage));
                        memcpy(pLAddr, temp_addr->ifa_addr, sizeof(struct sockaddr_storage));
                        bret = true;
                        break;
                    }
                }
            }else if ((strcmp(temp_addr->ifa_name, "pdp_ip0") == 0)){ //"pdp_ip0" 蜂窝网络 (iOS)
                  if (temp_addr->ifa_addr->sa_family == AF_INET) {
                    char ipv4Addr[INET_ADDRSTRLEN] = {0};
                    struct sockaddr_in *ipv4 = (struct sockaddr_in *)temp_addr->ifa_addr;
                    if (fd_format_ipv4_address(&ipv4->sin_addr, ipv4Addr) &&
                        (strcmp(ipv4Addr, "") != 0)) {
                        memset(pLAddr, 0, sizeof(struct sockaddr_storage));
                        memcpy(pLAddr, temp_addr->ifa_addr, sizeof(struct sockaddr_storage));
                        bret = true;
                    }
                }
                else if (temp_addr->ifa_addr->sa_family == AF_INET6){
                    char ipv6Addr[INET6_ADDRSTRLEN] = {0};
                    struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)temp_addr->ifa_addr;
                    if (fd_format_ipv6_address(&ipv6->sin6_addr, ipv6Addr) &&
                        (strcmp(ipv6Addr, "") != 0) &&
                        (strncasecmp(ipv6Addr, "fe80", strlen("fe80")) != 0)) {
                        memset(pLAddr, 0, sizeof(struct sockaddr_storage));
                        memcpy(pLAddr, temp_addr->ifa_addr, sizeof(struct sockaddr_storage));
                        bret = true;
                        break;
                    }
                }
            }
        
        temp_addr = temp_addr->ifa_next;
    }
    
    freeifaddrs(interfaces);
    
    return bret;
}

void fd_dump_socket(fd_socket iSocket, char *pFlag){
    char ipstr[INET6_ADDRSTRLEN] = {0};
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof (struct sockaddr_storage);
    if (getsockname(iSocket, (struct sockaddr *)&addr, (socklen_t*)&addr_len) == 0) {
        if (addr.ss_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)&addr;
            fd_format_ipv4_address(&ipv4->sin_addr, ipstr);
            fd_log("[%s]log socket:{\nfamily: AF_INET\nhost: %s\nport: %d\n}",
                   pFlag, ipstr, ntohs(ipv4->sin_port));
        }
        else if (addr.ss_family == AF_INET6){
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&addr;
            fd_format_ipv6_address(&ipv6->sin6_addr, ipstr);
            fd_log("[%s]log socket:{\nfamily: AF_INET6\nhost: %s\nport: %d\n}",
                   pFlag, ipstr, ntohs(ipv6->sin6_port));
        }
    }
}

void fd_dump_sockaddr(struct sockaddr_storage *pAddr, char *pFlag){
    if (pAddr->ss_family == AF_INET) {
        char strip[INET_ADDRSTRLEN];
        struct sockaddr_in *ipv4 = (struct sockaddr_in *)pAddr;
        fd_format_ipv4_address(&ipv4->sin_addr, strip);
        fd_log("[%s]log addr:{\nfamily: AF_INET\nip: %s\nport: %d\n}",
               pFlag, strip, htons(ipv4->sin_port));
    }
    else if (pAddr->ss_family == AF_INET6){
        char strip[INET6_ADDRSTRLEN];
        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)pAddr;
        fd_format_ipv6_address(&ipv6->sin6_addr, strip);
        fd_log("[%s]log addr:{\nfamily: AF_INET6\nip: %s\nport: %d\n}",
               pFlag, strip, htons(ipv6->sin6_port));
    }
}

sa_family_t fd_get_local_family(void){
    sa_family_t ifamily = AF_UNSPEC;
    struct ifaddrs *interfaces = NULL;
    struct ifaddrs *temp_addr = NULL;
    
    if (getifaddrs(&interfaces) == 0) {  // 0 表示获取成功
        temp_addr = interfaces;
        while (temp_addr != NULL) {
           // fd_log("temp_addr->ifa_name: %s", temp_addr->ifa_name);
#if defined(__ANDROID__)
            if (strcmp(temp_addr->ifa_name, "wlan0") == 0)         //"wlan0" wifi (Android)
#else
                if ((strcmp(temp_addr->ifa_name, "en0") == 0) ||       //"en0" wifi (iOS)
                    (strcmp(temp_addr->ifa_name, "en1") == 0) ||       //"en1" wifi & 网线 (macOS))
                    (strcmp(temp_addr->ifa_name, "pdp_ip0") == 0))     //"pdp_ip0" 蜂窝网络 (iOS)
#endif
                {
                    if (temp_addr->ifa_addr->sa_family == AF_INET){
                        char ipv4Addr[INET_ADDRSTRLEN] = {0};
                        struct sockaddr_in *ipv4 = (struct sockaddr_in *)temp_addr->ifa_addr;
                        if (fd_format_ipv4_address(&ipv4->sin_addr, ipv4Addr)) {
                            ifamily = AF_INET;
                            fd_log("temp_addr->ifa_name: %s",ipv4Addr);
                        }
                    }
                    else if (temp_addr->ifa_addr->sa_family == AF_INET6){
                        char ipv6Addr[INET6_ADDRSTRLEN] = {0};
                        struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)temp_addr->ifa_addr;
                        if (fd_format_ipv6_address(&ipv6->sin6_addr, ipv6Addr)) {
                            ifamily = AF_INET6;
                            if ((strcmp(ipv6Addr, "") != 0) &&
                                (strncasecmp(ipv6Addr, "fe80", strlen("fe80")) != 0)) {
                                break;
                            }
                        }
                    }
                }
            
            temp_addr = temp_addr->ifa_next;
        }
    }
    
    freeifaddrs(interfaces);
    
    return ifamily;
}

void fd_free_sockaddr(struct sockaddr_storage **pAddr){
    if (*pAddr) {
        free(*pAddr);
        *pAddr = NULL;
    }
}

struct sockaddr_storage * fd_creat_local_sockaddr(uint16_t iPort){
    if (iPort <= 0x400 ) {
        fd_log("%s: illegal parameter", __func__);
        return NULL;
    }
    
    struct sockaddr_storage pladdr = {0};
    sa_family_t ifamily = fd_get_local_family();
    if (ifamily == AF_INET) {
        struct sockaddr_in *sockaddr4 = (struct sockaddr_in *)&pladdr;
#if defined(__APPLE__)
        sockaddr4->sin_len         = sizeof(struct sockaddr_in);
#endif
        sockaddr4->sin_family      = AF_INET;
        sockaddr4->sin_port        = htons(iPort);
        sockaddr4->sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else if (ifamily == AF_INET6){
        struct sockaddr_in6 *sockaddr6 = (struct sockaddr_in6 *)&pladdr;
#if defined(__APPLE__)
        sockaddr6->sin6_len       = sizeof(struct sockaddr_in6);
#endif
        sockaddr6->sin6_family    = AF_INET6;
        sockaddr6->sin6_port      = htons(iPort);
        sockaddr6->sin6_addr      = in6addr_any;
    }
    else{
        fd_log("%s: protocol not supported", __func__);
        return NULL;
    }
    
    struct sockaddr_storage * pLocalAddr = (struct sockaddr_storage *)malloc(sizeof(struct sockaddr_storage));
    if (!pLocalAddr) {
        fd_log("%s: malloc failed", __func__);
        return NULL;
    }
    
    memset(pLocalAddr, 0, sizeof(struct sockaddr_storage));
    memcpy(pLocalAddr, &pladdr, sizeof(struct sockaddr_storage));
    
    return pLocalAddr;
}

struct sockaddr_storage * fd_creat_remote_sockaddr(char *pHost, uint16_t iPort){
    if (!pHost) {
        fd_log("%s: illegal parameter", __func__);
        return NULL;
    }
    
    struct sockaddr_storage praddr = {0};
    struct addrinfo hints;
    struct addrinfo *res, *cur;
    memset(&hints, 0, sizeof(struct addrinfo));
    

   //过滤器设置
#if defined(__ANDROID__)
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_IP;   // IP协议
#else
    hints.ai_family =  AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_DEFAULT;
    hints.ai_protocol = IPPROTO_IP;   // IP协议
#endif
    int status = getaddrinfo(pHost, "8000", &hints, &res);
    if (status != 0) {
        fd_log("%s: %s(%d)", __func__, strerror(status), status);

        return NULL;
    }
    
    bool bflag = false;
    for (cur = res; cur; cur = cur->ai_next) {
        if (cur->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)&praddr;
            memcpy(ipv4, (struct sockaddr_in *)cur->ai_addr, sizeof(struct sockaddr_in));
            ipv4->sin_port = htons(iPort);
            
            bflag = true;
        }
        else if (cur->ai_family == AF_INET6){
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&praddr;
            memcpy(ipv6, (struct sockaddr_in6 *)cur->ai_addr, sizeof(struct sockaddr_in6));
            ipv6->sin6_port = htons(iPort);
            
            bflag = true;
        }
    }
    freeaddrinfo(res);
    
    if (!bflag) {
        fd_log("%s: protocol not supported", __func__);
        return NULL;
    }
    
    struct sockaddr_storage *pRemoteAddr = (struct sockaddr_storage *)malloc(sizeof(struct sockaddr_storage));
    if (!pRemoteAddr) {
        fd_log("%s: malloc failed", __func__);
        return NULL;
    }
    
    memset(pRemoteAddr, 0, sizeof(struct sockaddr_storage));
    memcpy(pRemoteAddr, &praddr, sizeof(struct sockaddr_storage));
    
    return pRemoteAddr;
}
static pthread_mutex_t fd_mutex;

//SOCK_STREAM(TCP), SOCK_DGRAM(UDP)
fd_socket fd_creat_socket(int iType){
    if (iType != SOCK_STREAM && iType != SOCK_DGRAM) {
        fd_log("%s: itype not supported", __func__);
        return INVALID_SOCKET;
    }
    
    int nosigpipe = 1;
    int reuseOn = 1;
    int status = SOCKET_ERROR;
    int isocket = INVALID_SOCKET;
    
    pthread_mutex_lock(&fd_mutex);
    sa_family_t ifamily = fd_get_local_family();
    if (ifamily == AF_INET6) {
        isocket = socket(AF_INET6, iType, 0);
    }
    else if (ifamily == AF_INET){
        isocket = socket(AF_INET, iType, 0);
    }
    else{
        pthread_mutex_unlock(&fd_mutex);
        fd_log("%s error: protocol not supported", __func__);
        return INVALID_SOCKET;
    }
    pthread_mutex_unlock(&fd_mutex);
    
    if (isocket <0 ) {
        return  INVALID_SOCKET;
    }
    
    /*!
     *屏蔽SIGPIPE信号,等同于signal(SIGPIPE,SIG_IGN);
     *SIGPIPE信号
     *在linux下写socket的程序的时候，如果尝试send到一个disconnected socket上，就会让底层抛出一个SIGPIPE信号。
     *该信号的缺省处理方法是退出进程。
     */
    //区分安卓和ios的 设置setsockopt 的区别
    
#if defined(__ANDROID__)
    struct linger optval1;
    optval1.l_onoff = 1;
    optval1.l_linger = 1000;
    status = setsockopt(isocket, SOL_SOCKET, SO_LINGER, &optval1, sizeof(optval1));// IP协议
#else
    status = setsockopt(isocket, SOL_SOCKET, SO_NOSIGPIPE, &nosigpipe, sizeof(nosigpipe));
#endif
    
    if (status == SOCKET_ERROR) {
        fd_log("%s error: shielded signal sigpipe failed", __func__);
        close(isocket);
        return INVALID_SOCKET;
    }
    
    
    //地址复用，解决端口被占用问题
#if defined(__ANDROID__)
    struct linger optval2;
    optval2.l_onoff = 1;
    optval2.l_linger = 1000;
    status = setsockopt(isocket, SOL_SOCKET, SO_REUSEADDR, &optval2, sizeof(optval2));
#else
    status = setsockopt(isocket, SOL_SOCKET, SO_REUSEADDR, &reuseOn, sizeof(reuseOn));
#endif
   
    if (status == SOCKET_ERROR) {
        fd_log("%s error: enabling address reuse failed", __func__);
        close(isocket);
        return INVALID_SOCKET;
    }
    
    return isocket;
}

void fd_close_socket(fd_socket *iSocket){
    if (*iSocket != INVALID_SOCKET) {
        shutdown(*iSocket, SHUT_RDWR);
        close(*iSocket);
        *iSocket = INVALID_SOCKET;
    }
}

bool fd_bind(fd_socket iSocket, struct sockaddr *pLAddr){
    if (iSocket == INVALID_SOCKET || !pLAddr) {
        fd_log("%s error: illegal parameter", __func__);
        return false;
    }
#if defined(__APPLE__)
    int iret = bind(iSocket, pLAddr, pLAddr->sa_len);
#else
    int iret = bind(iSocket, pLAddr, sizeof(struct sockaddr));
#endif
    if (iret == SOCKET_ERROR) {
        fd_log("%s error: %s(%d)", __func__, strerror(errno), errno);
        return false;
    }
    
    return true;
}

bool fd_listen(fd_socket iSocket, uint32_t iMaxCount){
    if (iSocket == INVALID_SOCKET) {
        fd_log("%s error: illegal parameter", __func__);
        return false;
    }
    
    int iret = listen(iSocket, iMaxCount);
    if (iret == SOCKET_ERROR) {
        fd_log("%s error: %s(%d)", __func__, strerror(errno), errno);
        return false;
    }
    
    return true;
}

fd_socket fd_accept(fd_socket iSocket){
    if (iSocket == INVALID_SOCKET) {
        fd_log("%s error: illegal parameter", __func__);
        return false;
    }
    
    struct sockaddr client = {0};
    socklen_t addrLen = sizeof(socklen_t);
    fd_socket rwfd = accept(iSocket, &client, &addrLen);
    if (rwfd == SOCKET_ERROR){
        fd_log("%s error: %s(%d)", __func__, strerror(errno), errno);
    }
    
    return rwfd;
}

bool fd_connect(fd_socket iSocket, struct sockaddr *pRaddr, uint32_t iTimeout){
    if (iSocket == INVALID_SOCKET || !pRaddr) {
        fd_log("%s error: illegal parameter", __func__);
        return false;
    }
    
    /*
     *1.设置成非阻塞模式来控制链接超时
     *2.成功链接后再设为阻塞模式
     */
    //1.设置非阻塞
    int status = SOCKET_ERROR;
    int flags = fcntl(iSocket, F_GETFL,0);
    fcntl(iSocket, F_SETFL, flags | O_NONBLOCK);
#if defined(__APPLE__)
    status = connect(iSocket, pRaddr, pRaddr->sa_len);
#else
    status = connect(iSocket, pRaddr, sizeof(struct sockaddr));
#endif
    if (status == SOCKET_ERROR){
        if (errno == EINPROGRESS){
            //it is in the connect process
            fd_set fdwrite;
            int error;
            int len = sizeof(int);
            
            /**
             * struct timeval tv是select的超时时间，它可以使select处于三种状态:
             * 1.若将NULL以形参传入，即不传入时间结构，就是将select置于阻塞状态，一定等到监视文件描述符集合中某个文件描述符发生变化为止；
             * 2.若将时间值设为0秒0毫秒，就变成一个纯粹的非阻塞函数，不管文件描述符是否有变化，都立刻返回继续执行，文件无变化返回0，有变化返回一个正值；
             * 3.tv的值大于0，这就是等待的超时时间，即select在timeout时间内阻塞，超时时间之内有事件到来就返回了，否则在超时后不管怎样一定返回，返回值同上述。
             */
            struct timeval tv;
            tv.tv_sec = iTimeout / 1000;
            tv.tv_usec = (iTimeout % 1000) * 1000;
            
            FD_ZERO(&fdwrite);
            FD_SET(iSocket, &fdwrite);
            
            int ret = select(iSocket + 1, NULL, &fdwrite, NULL, &tv);
            //int ret =  select(iSocket + 1, &fdwrite, NULL, NULL, &tv);
            if (ret <= 0) {//err
                fd_log("%s error111: %s(%d)", __func__, strerror(errno), errno);
                return false;
            }
            //判断是否链接成功  获取套接字的选项
            getsockopt(iSocket, SOL_SOCKET, SO_ERROR, &error, (socklen_t *)&len);
            
            if (error != 0) {
                fd_log("%s error1111111: %s(%d)", __func__, strerror(error), error);
                return false;
            }
           
        }
        else{
            fd_log("%s error2222: %s(%d)", __func__, strerror(errno), errno);
        
            return false;
        }
    }
    
    //2.连接成功后设置阻塞模式
    flags = fcntl(iSocket, F_GETFL,0);
    flags &= ~ O_NONBLOCK;
    fcntl(iSocket,F_SETFL, flags);
    
    return true;
}

ssize_t fd_recv(fd_socket iSocket, char *pData, size_t iLen, uint32_t iTimeout){
    if (iSocket == INVALID_SOCKET || !pData) {
        fd_log("%s error: illegal parameter", __func__);
        return SOCKET_ERROR;
    }
   
    ssize_t ret;
    ssize_t rlen = 0;
    ssize_t rslt = 0;
   
    fd_set    readfds;
    while (rlen < iLen){
        struct timeval tv;
     
        tv.tv_sec = iTimeout / 1000;
        tv.tv_usec = (iTimeout % 1000) * 1000;
   
        FD_ZERO(&readfds);
        FD_SET(iSocket, &readfds);
      
        ret = select(iSocket + 1, &readfds, NULL, NULL, &tv);
        if (ret < 0) {// err
            fd_log("%s error: %s(%d)", __func__, strerror(errno), errno);
            return SOCKET_ERROR;
        }
        else if (ret == 0){// time out
            //fd_log("%s error:recv timeout", __func__);
            return 0;
        }
        if (FD_ISSET(iSocket, &readfds)){  //测试iSock是否可读
             rslt = (int32_t)recv(iSocket, pData + rlen, iLen - rlen, 0);
             //printf("recv*****5555555\n");
            if (rslt <= 0) {//err
                fd_log("%s error: %s(%d)", __func__, strerror(errno), errno);
                return SOCKET_ERROR;
            }
            else{
                rlen += rslt;
            }
        }
    }
    return rlen;
}

ssize_t fd_send(fd_socket iSocket, char *pData, size_t iLen){
    if (iSocket == INVALID_SOCKET || !pData) {
        fd_log("%s error: illegal parameter", __func__);
        return SOCKET_ERROR;
    }
    
    ssize_t wlen = 0;
    ssize_t rlen;
    while (wlen < iLen){
        rlen = send(iSocket, pData + wlen, iLen - wlen, 0);
        if (rlen <= 0) {
            fd_log("%s error: %s(%d)", __func__, strerror(errno), errno);
            return SOCKET_ERROR;
        }
        else {
            wlen += rlen;
        }
    }
    return wlen;
}

bool fd_recvfrom(fd_socket iSocket, char *pData, ssize_t *iLen, struct sockaddr *pRaddr, uint32_t iTimeout){
    
    ssize_t originalSize = *iLen;
    if (iSocket == INVALID_SOCKET || !pData || !pRaddr ) {
        fd_log("%s: illegal parameter", __func__);
        return false;
    }
    
    socklen_t addrLen = 0;
    sa_family_t ifamily = fd_get_local_family();
    if (ifamily == AF_INET) {
        addrLen = sizeof(struct sockaddr_in);
    }
    else if (ifamily == AF_INET6) {
        addrLen = sizeof(struct sockaddr_in6);
    }
    else{
        fd_log("%s: unknown ifamily", __func__);
        return false;
    }
    
    struct timeval tv;
    tv.tv_sec = iTimeout / 1000;
    tv.tv_usec = (iTimeout % 1000) * 1000;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(iSocket, &readfds);
    int irslt = select(iSocket + 1, &readfds, NULL, NULL, &tv);
    if (irslt <= 0 ){
        fd_log("%s: irslt(%d) error[%s(%d)]", __func__, irslt, strerror(errno), errno);
        return false;
    }
    
    if (FD_ISSET (iSocket, &readfds)){
        *iLen = recvfrom(iSocket, pData, originalSize, 0, pRaddr, &addrLen);
        if (*iLen <= 0){
            fd_log("%s: %s(%d)", __func__, strerror(errno), errno);
            return false;
        }
        
        if ((*iLen) + 1 >= originalSize){
            fd_log("%s: received a message that was too large", __func__);
            return false;
        }
        
        pData[*iLen]=0;
        
        return true;
    }
    
    return false;
}

bool fd_sendto(fd_socket iSocket, char *pData, size_t iLen, struct sockaddr *pRaddr){
    if (iSocket == INVALID_SOCKET || !pData || !pRaddr){
        fd_log("%s: illegal parameter", __func__);
        return false;
    }
    
#if defined(__APPLE__)
    ssize_t iret = sendto(iSocket, pData, iLen, 0, pRaddr, pRaddr->sa_len);
#else
    ssize_t iret = sendto(iSocket, pData, iLen, 0, pRaddr, sizeof(struct sockaddr));
#endif
    if (iret != iLen) {
        fd_log("%s: %s(%d)", __func__, strerror(errno), errno);
        return false;
    }
    
    return true;
}

