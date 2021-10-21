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

#ifndef RUDPSOCKET_HPP_HEADER_INCLUDED
#define RUDPSOCKET_HPP_HEADER_INCLUDED

#include <RSocket.hpp>
#include <RStream.hpp>
#include <RNetAddress.hpp>

namespace athena
{

class ATHENA_EX RUdpSocket : public RSocket
{
public:
	/*
	 *	params:
	 *		addrFamily 	AF_INET		IPv4
	 *					AF_INET6	IPv6
	 *	如果客户端没有提供localAddress,则不绑定ip或port,直到Connect或者send的时候再随机分配ip:port
	 *	否则绑定具体的ip:port
	 */
	RUdpSocket(int addrFamily = AF_INET, bool bAutoClose = true);

	RUdpSocket(const RNetAddress& localAddress, bool bAutoClose = true);

	virtual ~RUdpSocket(); 

    /*
     *不会建立一个"真实"的链接，只是让内核保存对方的IP地址和端口号;
     *不会向对方发送任何数据报，如果有些错误如端口不可达或指定端口没有服务器，调用者在向对方发出数据报前不能发现
     */
	bool Connect(const RString& host, int nPort);

	bool Connect(const RNetAddress& remoteAddr);

    bool IsConnected() const;

    /*
     *  对于已连接socket
     */
	int Send(const char* pData, int nSize);

    /*
     *  praams:
     *      buf\stream 待传送字符串
     *      nbytes  数据报长度
     *      ip、port    传送远程IP和端口
     *      waitMilliseconds    等待socket可读的超时间隔，单位：毫秒
     *          >   等待时间，超时socket还不可写，则返回失败
     *          =0  不等待，socket不可写就返回失败
     *          -1  一直等到socket可写
     *  return:
     *      SOCKET_ERROR    出错
     *      >=0             发送字节数
     */
	int SendTo(const char* buf, int nbytes, 
	           const char* ip, int port, 
	           int waitMilliseconds = -1);

	int SendTo(const char* buf, int nbytes, 
	           const RNetAddress& remoteAddr,
	           int waitMilliseconds = -1);

    int SendTo(const RString& buf, 
               const RNetAddress& remoteAddr,
               int waitMilliseconds = -1);

    int SendTo(const RStream& stream, 
               const RNetAddress& remoteAddr,
               int waitMilliseconds = -1);

    /*
     *  params:
     *      buf\stream  接收缓存
     *      nbytes      buf的长度,可接收的最大长度，单位：字节
     *      ip\port     远程IP地址和端口
     *      waitMilliseconds    等待socket可读的超时间隔
     *          >0  等待时间，超时还没有数据，则返回失败
     *          =0  不等待，socket不可读即返回失败
     *          -1  一直等到socket可读
     *  return:
     *      SOCKET_ERROR    出错
     *      >0              接收的字节数
     *      =0              已连接socket被关闭
     */
	int RecvFrom(char* buf, int nbytes,
	             char* ip = NULL, int* port = NULL,
	             int waitMilliseconds = -1);

	int RecvFrom(char* buf, int nbytes, 
	             RNetAddress& remoteAddr, 
	             int waitMilliseconds = -1);

    int RecvFrom(RString& buf, int nbytes,
                 RNetAddress& remoteAddr,
                 int waitMilliseconds = -1);

    int RecvFrom(RStream& stream, int nbytes,
                 RNetAddress& remoteAddr,
                 int waitMilliseconds = -1);

private:
    bool            m_isConnected;

};

}   /*namespace*/

#endif /*RUDPSOCKET_HPP_HEADER_INCLUDED*/
