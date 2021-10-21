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

#ifndef RNETUNIDEF_HPP_HEADER_INCLUDED
#define RNETUNIDEF_HPP_HEADER_INCLUDED

#include <RString.hpp>
/*
 *  socket pram
 */
#if defined(WIN32) || defined(_WINDOWS) || defined(_WIN32_WCE)
 #include <winsock2.h>
 #include <Ws2tcpip.h>
 #include <windows.h>
#else
 #include <netdb.h>
 #include <ctype.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 #include <arpa/inet.h>
 #include <unistd.h>
 #include <sys/time.h>
 #include <sys/ioctl.h>
 #include <fcntl.h>
 #include <errno.h>
#endif

#ifdef _WIN32_WCE
#include <Winbase.h>
#endif

namespace athena
{
#if defined(WIN32) || defined(_WINDOWS) || defined(_WIN32_WCE)
  #ifndef CloseSocket
  #define CloseSocket(s) ::closesocket(s)
  #endif
  ATHENA_EX RString GetSockError(int errnum);
#else
  #ifndef CloseSocket
  #define CloseSocket(s) ::close(s)
  #endif
#endif

#define	MAXSOCKADDR  128	/* max socket address structure size */

ATHENA_EX int __startupWSock();

ATHENA_EX void __cleanupWSock();

/*
 *  获取主机字节序
 *  return 
 *      0   little-endian   小端字节序，低序字节存储在起始地址
 *      1   big-endian      大端字节序，高序字节存储在其实地址
 *      -1  获取主机字节序失败
 */
ATHENA_EX int GetByteOrder();

}       /*namespace*/

#endif  /*RNETUNIDEF_HPP_HEADER_INCLUDED*/
