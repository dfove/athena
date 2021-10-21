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

#ifndef RSOCKET_HPP_HEADER_INCLUDED
#define RSOCKET_HPP_HEADER_INCLUDED

#include <RString.hpp>
#include <RNetUnidef.hpp>
#include <RNetAddress.hpp>
#include <time.h>

namespace athena
{
#if defined(WIN32) || defined(_WINDOWS)
#else
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR -1
  typedef int SOCKET;
#endif //!WIN32

typedef enum _SHUTDOWN_TYPE
{
#if defined(WIN32) || defined(_WINDOWS)
    SHUTDOWN_NONE = -1,
    SHUTDOWN_READ = SD_RECEIVE,
    SHUTDOWN_WRITE = SD_SEND,
    SHUTDOWN_BOTH = SD_BOTH
#else
    SHUTDOWN_NONE = -1,
    SHUTDOWN_READ = SHUT_RD,
    SHUTDOWN_WRITE = SHUT_WR,
    SHUTDOWN_BOTH = SHUT_RDWR
#endif
}SHUTDOWN_TYPE;

typedef enum 
        {
            FD_Readable = 1,
            FD_Writeable = 2,
            FD_Errable = 4
        }
        MYFDSET;

class ATHENA_EX RSocket
{
public:
    virtual ~RSocket();

/*
 *  SOCKET Operation
 */
public:
    /*
     *	参数family,socktype,protocol参见系统函数socket的参数说明
     *	family:
     *		AF_INET
     *		AF_INET6 
     *		AF_LOCAL
     *		AF_ROUTE
     *		AF_KEY
     *	socktype:
     *		SOCK_STREAM
     *		SOCK_DGRAM
     *		SOCK_RAW
     *	protocol:
     *		一般设置为0，除非用到原始套接口上
     *	功能:创建指定协议的socket
     *		如果socket已创建，则返回已存在的socket fd
     *		如果新创建成功，则返回创建的socket fd
     *		出错，返回SOCKET_ERROR(-1)
     */
    SOCKET Create(int family = AF_INET, int socktype = SOCK_STREAM,int protocol = 0);

//    /*
//     *  设置套接字，如果之前套接字还有效将关闭套接字，并设置为新的套接口fd
//     */
//    bool SetSocketFd(SOCKET fd, 
//                     int socktype, 
//                     int protocol = 0,
//                     bool blocktype = true,
//                     bool autoClose = true);

	SOCKET GetSocketFd() const;

    bool IsValid() const;

    /*
     *  对于TCP/UDP，可绑定IP和端口，一般主要是绑定端口
     *  params:
     *      ip      IP地址，TCP/UDP如果为NULL，则绑定通配地址
     *                      Raw socket不能为空
     *      port    端口，TCP/UDP，如果<=0，则有内核指定端口
     *                      Raw socket无效
     */
    bool Bind(int port, const char* ip = NULL);

    bool Bind(const SockAddr* addr, socklen_t addrlen);

    bool Bind(const RNetAddress& localAddr);

    virtual bool Close();

    bool IsClosed() const;

    void Shutdown(SHUTDOWN_TYPE type);

    SHUTDOWN_TYPE GetShutdownType() const;

	/*
	 *	与套接口关联的本地协议地址
	 *		1、不调用bind，当connect成功后，关联的本地IP地址和本地端口号
	 *		2、以本地端口0调用bind后，关联的本地端口号
	 *		3、作为服务器，捆绑了通配IP地址，与客户建立连接后(accept成功)，分配给此链接的本地IP地址
	 *
     *  wrapper of getsockname
     *      windows:在绑定本地地址和端口之后有效，没有bind则返回失败
     *      linux  :没有bind也有效，返回的地址为通配地址，端口为0
     *  取得与套接口关联的本地协议地址
     *  return:
     *      套接口的地址族,出错时返回-1
     */
    int GetSockName(RNetAddress& sockname);

	/*
	 *	与套接口关联的远程协议地址,在与远程建立连接之后才有
	 *
     *  wrapper of getpeername
     *      没有connect，则返回失败
     *  取得与套接口关联的远程协议地址
     *  return:
     *      套接口的地址族,出错时返回-1
     */
    int GetPeerName(RNetAddress& peername);

    /*
     *  在预订的时间内是否有数据可读写或出现异常typedef enum 
     *  {
     *      FD_Readable = 1,
     *      FD_Writeable = 2,
     *      FD_Errable = 4
     *  }
     *  MYFDSET;
     *  params:
     *      myfdset     enum结构MYFDSET的一项或者多项的或值,
     *                  如要判断是否可读写，为: FD_Readable | FD_Writeable
     *      millisecond 毫秒
     *                  -1、<0  永久等待，直到可读写或出现异常
     *                  0   不等待，判断是否可读写或异常后直接返回结果
     *                  >0  等待的时间，判断在预定时间内是否有数据可读写或异常
     *      [out]readable   如果可读，则赋值true
     *      [out]writeable  如果可写，则赋值true
     *      [out]errable    如果异常，则赋值true
     *  return:
     *      设定的myfdset符合要求，返回true，否则返回false
     *      值-结果参数的返回也仅限于myfdset的范围
     *      如:myfdset == FD_Readable | FD_Writeable,即使出现异常,*errable也不做改变
     */
    bool Select(int myfdset, 
                int millisecond = -1, 
                bool* readable = NULL,
                bool* writeable = NULL,
                bool* errable = NULL);

    void AddBytesSent(unsigned long sentBytes);

	unsigned long GetBytesSent(bool clear = false);

    void AddBytesRecved(unsigned long recvedBytes);

    unsigned long GetByteRecved(bool clear = false);

	time_t CreatTime() const;

    time_t CloseTime() const;

	void SetError(int syserr);

	bool IsError() const;

	const RString& ReportError();

	void ResetError();

/*
 *  SOCKET Optional and IP Optional
 */
public:
    /*
     *  设置socket是否阻塞
     *  params:
     *      bBlock  true    设置为阻塞
     *              false   设置为非阻塞
     */
	bool SetBlockType(bool bBlock); 

    /*
     *  返回socket是否非阻塞
     *  return:
     *      true    阻塞socket
     *      false   非阻塞socket
     */
    bool IsBlockType() const;

    bool SetSendBuf();

    bool SetRecvBuf();

    bool SetSockOpt(int level, int optname, const void* optval, socklen_t optlen);

protected:
    RSocket(int addressFamily, int socketType, int protocol = 0, bool bAutoClose = true);

    RSocket(SOCKET sockfd, int socktype, bool bBlockType = true, bool bAutoClose = true);

    RSocket(const RNetAddress & localAddr, int socketType, int protocol = 0, bool bAutoClose = true);

    /*
     *  return:
     *      套接口的地址族
     */
    int SockfdToFamily();

private:
    /*
     * copy constructor not available.
     * assignment operator not available.
     */
    RSocket(const RSocket& right);

    RSocket & operator=(const RSocket& right);

protected:
    SOCKET          m_socketFd;

    /*
     *  AF_INET
     *  AF_INET6
     *  Others
     */
    int             m_family;

    /*
     *  SOCK_DGRAM
     *  SOCK_STREAM
     *  Others
     */
    int             m_socketType;

    int             m_protocol;

    /*
     *  是否阻塞socket
     */
    bool            m_blocktype;

	/*
	 *	是否在对象销毁的时候关闭socket
	 */
	bool         	m_bAutoClose;

    SHUTDOWN_TYPE   m_shutdownType;

    unsigned long   m_totalBytesSent;

    unsigned long   m_totalBytesReceived;

    time_t          m_CreaTime;

    time_t          m_CloseTime;

    int             m_nErrno;

    RString         m_lasterror;
};

}   /*athena*/

#endif /*RSOCKET_HPP_HEADER_INCLUDED*/
