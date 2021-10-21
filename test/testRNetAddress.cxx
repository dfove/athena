/*
 * Copyright (c) 2021 dfove.com Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <RStream.hpp>
#include <RNetAddress.hpp>
#include <errno.h>
#include <RString.hpp>
#include <RNetUnidef.hpp>

using namespace athena;

int main(int argc, char** argv)
{
/**************************
test RNetAddress
**************************/    
    if ( argc != 3 )
    {
        printf("Usage:%s [ip] [service|port]\n", argv[0]);
        return 0;
    }

    int port = atoi(argv[2]);
    RNetAddress netaddress(argv[1], port, AF_INET, SOCK_STREAM);
    printf("errorno: %d, %s\n", netaddress.GetLastErrno(), netaddress.GetLastErrorDesc().c_str() );
    printf("num of the netaddress is %d\n", netaddress.GetNumOfAddr());
    netaddress.PrintNetAddress();
    printf("\n************************************\n");
    RNetAddress netaddress2 = netaddress;
    netaddress2 = netaddress;
    netaddress2.PrintNetAddress();

    printf("socket test.......................\n");
  #if defined(WIN32) || defined(_WINDOWS)
    int err;
    WSADATA wsdata;
    err = WSAStartup(MAKEWORD(2, 2), &wsdata);
    if (err != 0)
    {
        perror("startup error");
        return -1;
    }
  #else
    typedef int SOCKET;
  #endif
    SOCKET fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr, cli;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = 0;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if ( bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0 )
    {
        printf("bind error");
        return -1;
    }
    if ( bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0 )
    {
  #if defined(WIN32) || defined(_WINDOWS)
        err = WSAGetLastError();
        printf("bind2 errno is %d:%s\n", err, GetSockError(err).c_str());
  #else
        printf("errno is %d\n", errno);
        perror("bind2 error");
  #endif
        return -1;
    }
    socklen_t len = 128;
    int ret = getsockname(fd, (struct sockaddr*)&cli, &len);
    //int ret = getpeername(fd, (struct sockaddr*)&cli, &len);
    if ( ret == -1 )
    {
  #if defined(WIN32) || defined(_WINDOWS)
        err = WSAGetLastError();
        printf("errno is %d:%s\n", err, GetSockError(err).c_str());
  #else
        perror("getsockname error");
  #endif
        return -1;
    }
    printf("fd:%d, errno:%d, ret:%d, len is %d, family:%s, port:%d, ip:%s\n", 
            fd,
            errno,
            ret,
            len,
            cli.sin_family==AF_INET?"AF_INET":"unknow", 
            cli.sin_port, 
            inet_ntoa(cli.sin_addr));

    __cleanupWSock();

  #if defined(WIN32) || defined(_WINDOWS)
    char ctmp[256];
    strerror_s(ctmp, sizeof(ctmp), ERROR_TIMEOUT);
    printf("the error is %s, %s, get\n", 
            GetSockError(ERROR_TIMEOUT).empty()?"empty":GetSockError(ERROR_TIMEOUT).c_str(), ctmp);
    return 0;
  #endif
}
