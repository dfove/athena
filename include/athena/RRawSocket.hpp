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

#ifndef RRAWSOCKET_HPP_HEADER_INCLUDED
#define RRAWSOCKET_HPP_HEADER_INCLUDED

#include <RSocket.hpp>
#include <RStream.hpp>
#include <RNetAddress.hpp>

namespace athena
{

class ATHENA_EX RRawSocket : public RSocket
{
public:
    /*
     *	功能:创建raw socket
     *
     *	参数family,socktype,protocol参见系统函数socket的参数说明
     *	family:
     *		AF_INET
     *		AF_INET6 
     *      AF_ROUTE
     *      AF_KEY
     *  protocol:
     *      IPPROTO_ICMP
     *      IPPROTO_IGMP
     *	bAutoClose:
     *		销毁此类时候自动关闭相应的socket fd
     */
    RRawSocket(int addressFamily, int protocol, bool bAutoClose = true);

    virtual ~RRawSocket();

public:
	int SendTo(const char* buf, int nbytes, const char* ip);

	int SendTo(const char* buf, int nbytes, const RNetAddress& remoteAddr);

	int SendTo(const RString& str, const RNetAddress& remoteAddr);

	int SendTo(const RStream& stream, const RNetAddress& remoteAddr);

	int RecvFrom(char* buf, int nbytes, char* ip = NULL);

	int RecvFrom(char* buf, int nbytes, RNetAddress& remoteAddr);

	int RecvFrom(RString& str, RNetAddress& remoteAddr);

	int RecvFrom(RStream& stream, RNetAddress& remoteAddr);
private:
    
};

}   //namespace

#endif      //RRAWSOCKET_HPP_HEADER_INCLUDED
