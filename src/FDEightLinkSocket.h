//
//  FDEightLinkSocket.h
//  EightEyeLink
//
//  Created by lei.FY on 2019/6/5.
//  Copyright © 2019 Lei.FY. All rights reserved.
//

#ifndef FDEightLinkSocket_h
#define FDEightLinkSocket_h


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/fcntl.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#if defined(__ANDROID__) && __ANDROID_API__ < 24
#include "ifaddrs-android-4dage.h"
#else
#include <ifaddrs.h>
#endif

#if defined(__cplusplus)
extern "C"
{
#endif
    
#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif
    
#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif
    
    typedef int fd_socket;
    
    bool fd_is_ipv6addr(const char *pAddr);
    bool fd_format_ipv4_address(struct in_addr *ipv4Addr, char *pAddr);
    bool fd_format_ipv6_address(struct in6_addr *ipv6Addr, char *pAddr);
    bool fd_get_address_by_host(char *pHost, char *pAddr);
    bool fd_get_local_ipaddr(char *pLAddr);
    bool fd_get_local_addrInfo(struct sockaddr_storage *pLAddr);
    
    void fd_dump_socket(fd_socket iSocket, char *pFlag);
    void fd_dump_sockaddr(struct sockaddr_storage *pAddr, char *pFlag);
    
    sa_family_t fd_get_local_family(void); // AF_UNSPEC / AF_INET / AF_INET6
    
    void fd_free_sockaddr(struct sockaddr_storage **pAddr);
    struct sockaddr_storage * fd_creat_local_sockaddr(uint16_t iPort);
    struct sockaddr_storage * fd_creat_remote_sockaddr(char *pHost, uint16_t iPort);
    
    fd_socket fd_creat_socket(int iType);//SOCK_STREAM(TCP), SOCK_DGRAM(UDP)
    void fd_close_socket(fd_socket *iSocket);
    
    bool fd_bind(fd_socket iSocket, struct sockaddr *pLAddr);
    bool fd_listen(fd_socket iSocket, uint32_t iMaxCount);
    fd_socket fd_accept(fd_socket iSocket);
    bool fd_connect(fd_socket iSocket, struct sockaddr *pRaddr, uint32_t iTimeout);//ms
    
    ssize_t fd_recv(fd_socket iSocket, char *pData, size_t iLen, uint32_t iTimeout);//ms
    ssize_t fd_send(fd_socket iSocket, char *pData, size_t iLen);
    
    bool fd_recvfrom(fd_socket iSocket, char *pData, ssize_t *iLen, struct sockaddr *pRaddr, uint32_t iTimeout);//ms
    bool fd_sendto(fd_socket iSocket, char *pData, size_t iLen, struct sockaddr *pRaddr);
    
#if defined(__cplusplus)
}
#endif

#endif /* FDCSocket_h */
