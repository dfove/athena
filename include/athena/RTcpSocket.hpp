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

#ifndef RTCPSOCKET_HPP_HEADER_INCLUDED
#define RTCPSOCKET_HPP_HEADER_INCLUDED

#include <RSocket.hpp>
#include <RStream.hpp>

namespace athena
{

class ATHENA_EX RTcpSocket : public RSocket
{
public:
    typedef enum _TcpErrno {
        E_Success	=		0,          //成功
        E_Argv	    =       1,          //不正确的参数
        E_Create	=		2,          //创建socket失败
        E_Sockfd    =       3,          //无效的socket fd
        E_Bind		=		4,          //bind端口失败
        E_Listen	=		5,          //listen失败
        E_Connect	=		6,          //connect连接错误
        E_Accept    =       7,          //accept失败
        E_Send		=		8,          //send发送数据过程错误
        E_Recv		=		9,          //recv接收数据过程错误
        E_Notconn   =       10,         //没有连接
        E_Timeout	=		11,         //超时(指定时间间隔不可读写)
        E_WOULDBLOCK =      12,         //对于非阻塞
        E_Breaken	=		13,         //连接已断开
        E_Host		=		14,         //服务IP地址错误
        E_Port		=		15,         //端口错误
        E_Set		=		16,         //设置socket失败
        E_INTERRUPT =       17          //中断
    }TcpErrno;

    typedef enum _AddressFamily
    {
        IPV4 = 0,
        IPV6
    }MyIpType;

public:
    /*
     *	功能:创建tcp socket
     *
     *	参数family,socktype,protocol参见系统函数socket的参数说明
     *	family:
     *		AF_INET
     *		AF_INET6 
     *	bAutoClose:
     *		销毁此类时候自动关闭相应的socket fd
     */
    RTcpSocket(MyIpType addressFamily = IPV4, int protocol = 0, bool bAutoClose = true);

    /*
     *  params:
     *      sockfd  accept连接后的sockfd
     *      bBlockType 默认为阻塞方式true;非阻塞方式false
     *      bAutoClose  对象销毁的时候自动关闭sockfd
     */
    RTcpSocket(SOCKET sockfd, bool bBlock = true, bool bAutoClose = true);

    /*
     *  创建自动bind的socket
     *
     *  params:
     *      localAddr   自动绑定的本地地址
     *      bAutoClose  是否在销毁的时候自动关闭socket
     */
    RTcpSocket(const RNetAddress & localAddr, bool bAutoClose = true);

    virtual ~RTcpSocket();

    /*
     *  设置套接字，如果之前套接字还有效将作为参数返回改套接字id，并设置为新的套接口fd
     *  params:
     *      fd  新的已侦听或已连接套接字id
     *      bConnected  是否已经连接成功，否则为侦听
     *      addressFamily   IPV4 or IPV6
     *      protocol    协议字段
     *      bBlock      是否阻塞
     *      autoClose   是否自动关闭
     */
    int AttachSocketFd(SOCKET fd, 
                       bool bBlock = true,
                       bool bConnected = true,
                       MyIpType addressFamily = IPV4, 
                       int protocol = 0,
                       bool autoClose = true);

private:
    RTcpSocket(const RTcpSocket& right);

    RTcpSocket& operator=(const RTcpSocket& right);

public:
    /*
     *  如果socket还没有创建，则自动创建socket，并连接服务器地址和端口
     *  对于非阻塞socket，需设定超时时间，超过时间后即返回false
     *  对于阻塞socket,默认的超时时间大概为75秒(linux .etc)
     */
	bool Connect(const RString& host, int nPort, unsigned waitMilliseconds = 10*1000);

    bool Connect(const RNetAddress& remoteAddr, unsigned waitMilliseconds = 10*1000);

    virtual bool Close();

    bool IsConnected() const;

    /*
     *  已绑定或未绑定地址和端口
     *      如果未绑定则绑定指定IP和端口
     *      如果已绑定，则使用之前已绑定的地址和端口，忽略指定的地址和端口
     *  params:
     *      maxconn 最大连接数
     *      port    侦听端口.   
     *              默认值=-1   socket已经Bind
     *              端口值>0    socket没有Bind,在侦听之前要先绑定
     *      lpziIP  侦听IP，默认为INADDR_ANY
     *  return(_TcpErrno):
     *      E_Port
     *      E_Host
     *      E_Bind  如果之前没有调用Bind操作，在Listen中调用失败时产生
     *      E_Listen
     *      E_Success
     */
    int Listen(int maxconn, 
               int port = -1, 
               const char* lpszIP = NULL);

    /*
     *  return:
     *      socket fd
     */
    SOCKET Accept(const unsigned uMilliseconds = 10);                        //接受连接请求

    SOCKET Accept(RString& sIP, unsigned& uPort, const unsigned uMilliseconds = 10);    //接受连接请求，并得到对方的IP及port


    /*
     *  对于非阻塞侦听socket, 需设定超时时间，超过预定时间返回false
     *  对于阻塞侦听socket, 一直阻塞直到客户端已连接成功
     *  params:
     *      connfd          已连接socket
     *      waitMilliseconds    非阻塞accept等待时间，毫秒
     *      clientIP        远程连接的ip
     *      clientPort      远程连接的端口
     *  return(_TcpErrno):
     *      E_Sockfd
     *      E_Timeout
     *      E_Accept
     *      E_Success
     */
    int Accept(RTcpSocket& connfd, unsigned waitMilliseconds = 100);

    int Accept(RTcpSocket& connfd, RString& clientIP, int& clientPort, unsigned waitMilliseconds = 100);

    /*
     *  Send\Recv, 发送\接收最大nSize字节的数据
     *  return:
     *      >= 0 成功发送\接收的字节数
     *      SOCKET_ERROR  失败
     */
    int SendOnce(const char* pData, int nbytes);

    int SendOnce(const RString& buf);

    int SendOnce(const RStream& stream);

    int RecvOnce(char* pData, int nbytes);

    int RecvOnce(RString& buf, int nbytes);

    int RecvOnce(RStream& stream, int nbytes);

    /*
     *  Sendn\Recvn，发送、接收等于nSize字节的数据
     *  params:
     *      waitMilliseconds (Recvn同)
     *                   单位：毫秒
     *                   > 0    等待毫秒数，阻塞/非阻塞均相同，判断在某时间段内是否可读(recv)写(read)，
     *                          超时时间waitMilliseconds后返回E_Timeout
     *                   0      不等待，不可读写即返回失败E_Timeout
     *                   -1     对于阻塞一直等待，直到可读(recv)写(read)，
     *                          对于非阻塞，如果没有数据可读(recv)写(read)将返回E_WOULDBLOCK
     *  return(_TcpErrno):
     *      E_Sockfd
     *      E_Timeout
     *      E_WOULDBLOCK   //对于非阻塞
     *      E_Send
     *      E_Success
     */
	int Sendn(const char* pData, int nbytes, int waitMilliseconds = -1);

    int Sendn(const RString& buf, int waitMilliseconds = -1);

    int Sendn(const RStream& stream, int waitMilliseconds = -1);

    /*
     *  params:
     *      waitMilliseconds    同Sendn
     *  pData后不自动加上'\0'结尾
     *  return(_TcpErrno):
     *      E_Sockfd
     *      E_Timeout
     *      E_WOULDBLOCK    //对于非阻塞
     *      E_Recv
     *      E_Breaken
     *      E_Success
     */
	int Recvn(char* pData, int nbytes, int waitMilliseconds = -1);

    int Recvn(RString& buf, int nbytes, int waitMilliseconds = -1);

    int Recvn(RStream& stream, int nbytes, int waitMilliseconds = -1);

private:
    /*
     *  是侦听socket
     */
    bool    m_isListening;

    /*
     *  是已连接端口(客户端或服务端accept成功的socket)
     */
    bool    m_isConnected;

};

};  /*namespace*/

#endif /*RTCPSOCKET_HPP_HEADER_INCLUDED*/
