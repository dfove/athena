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

#include <RSocket.hpp>
#include <string.h>

using namespace athena;

RSocket::RSocket(int addressFamily, 
				 int socketType, 
				 int protocol /*= 0*/, 
				 bool bAutoClose /*= true*/)
		:m_socketFd(-1)
		,m_family(addressFamily)
		,m_socketType(socketType)
		,m_protocol(protocol)
        ,m_blocktype(true)
		,m_bAutoClose(bAutoClose)
		,m_shutdownType(SHUTDOWN_NONE)
		,m_totalBytesSent(0)
		,m_totalBytesReceived(0)
		,m_CreaTime(0)
		,m_CloseTime(0)
		,m_nErrno(0)
{
	Create(m_family, m_socketType, m_protocol);
}

RSocket::RSocket(SOCKET sockfd, int socktype, bool bBlockType /*= true*/, bool bAutoClose /*= true*/)
		:m_socketFd(sockfd)
		,m_family(-1)
		,m_socketType(socktype)
		,m_protocol(0)
        ,m_blocktype(bBlockType)
		,m_bAutoClose(bAutoClose)
		,m_shutdownType(SHUTDOWN_NONE)
		,m_totalBytesSent(0)
		,m_totalBytesReceived(0)
		,m_CreaTime(time(NULL))
		,m_CloseTime(0)
		,m_nErrno(0)
{
    if ( sockfd == INVALID_SOCKET )
    {
//      #if defined(WIN32) || (_WINDOWS)
//        m_nErrno = WSAEINVAL;       // Invalid argument 
//      #else
//        m_nErrno = EINVAL;      // Invalid argument 
//      #endif
//		SetError(m_nErrno);
    }
    else
    {
        m_family = SockfdToFamily();
        if ( m_family == -1 )
        {
          #if defined(WIN32) || (_WINDOWS)
            m_nErrno = WSAEINVAL;       // Invalid argument 
          #else
            m_nErrno = EINVAL;      // Invalid argument 
          #endif
    		SetError(m_nErrno);
        }
    }
    SetBlockType(m_blocktype);//设置阻塞模式socket
}

RSocket::RSocket(const RNetAddress & localAddr, 
				 int socketType, 
				 int protocol /*= 0*/, 
				 bool bAutoClose /*= true*/)
		:m_socketFd(-1)
		,m_family(localAddr.Family())
		,m_socketType(socketType)
		,m_protocol(protocol)
        ,m_blocktype(true)
		,m_bAutoClose(bAutoClose)
		,m_shutdownType(SHUTDOWN_NONE)
		,m_totalBytesSent(0)
		,m_totalBytesReceived(0)
		,m_CreaTime(0)
		,m_CloseTime(0)
		,m_nErrno(0)
{
    if ( m_socketType == 0 && localAddr.SockType() != 0 )
        m_socketType = localAddr.SockType();
    if ( m_protocol == 0 && localAddr.Protocol() != 0 )
        m_protocol = localAddr.Protocol();
	Create(m_family, m_socketType, m_protocol);
}

RSocket::~RSocket()
{
    if ( m_bAutoClose == true && IsValid() == true )
        CloseSocket(m_socketFd);
}

SOCKET 
RSocket::Create(int family /*= AF_INET*/, int socktype /*= SOCK_STREAM*/,int protocol /*= 0*/)
{
	if ( m_socketFd != -1 )
	{
	    Close();
    }
	m_socketFd = socket(family, socktype, protocol);
	if ( m_socketFd != INVALID_SOCKET )
	{
		m_family = family;
		m_socketType = socktype;
		m_protocol = protocol;
		m_CreaTime = time(NULL);
		m_CloseTime = 0;
	}
	else
	{
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = WSAGetLastError();
      #else
        m_nErrno = errno;
      #endif
        SetError(m_nErrno);
	}
	if ( m_blocktype == false ) //设置成非阻塞socket,因为默认为阻塞模式，可以不设置
	    SetBlockType(m_blocktype);

    //设置后bind,服务退出可快速重启(绑定的端口可快速释放)
    int n = 1;
    if (setsockopt(m_socketFd, SOL_SOCKET, SO_REUSEADDR, (const char*)(&n), sizeof(int)) < 0)
    {
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = WSAGetLastError();
      #else
        m_nErrno = errno;
      #endif
        SetError(m_nErrno);
    } 

	return m_socketFd;
}

//bool 
//RSocket::SetSocketFd(SOCKET fd, 
//                     int socktype, 
//                     int protocol /*= 0*/,
//                     bool blocktype /*= true*/,
//                     bool autoClose /*= true*/)
//{
//    if ( fd == INVALID_SOCKET )
//    {
//      #if defined(WIN32) || (_WINDOWS)
//        SetError(WSAEINVAL);        // Invalid argument 
//      #else
//        SetError(EINVAL);          // Invalid argument 
//      #endif
//      
//      return false;
//    }
//    if ( IsValid() == true )
//    {
//        Close();
//    }
//    m_socketFd = fd;
//    m_blocktype = blocktype;
//    m_family = SockfdToFamily();
//    m_socketType = socktype;
//    m_protocol = protocol;
//    m_bAutoClose = autoClose;
//    m_shutdownType = SHUTDOWN_NONE;
//    m_totalBytesSent = 0;
//    m_totalBytesReceived = 0;
//    m_CreaTime = time(NULL);
//    m_nErrno = 0;
//    m_lasterror.clear();
//    return true;
//}

SOCKET 
RSocket::GetSocketFd() const
{
	return m_socketFd;
}

bool 
RSocket::IsValid() const
{
	if ( m_socketFd >= 0 && !IsClosed())
		return true;
	return false;
}

bool 
RSocket::Bind(int port, const char* ip /*= NULL*/)
{
    if ( !IsValid() )
    {
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = WSAENOTSOCK;       // Invalid argument 
      #else
        m_nErrno = EBADF;      // Invalid argument 
      #endif
		SetError(m_nErrno);
        return false;
    }
    if ( port < 0 )
    {
        port = 0;
    }

	union 
	{
	  struct sockaddr	sa;
	  char				data[MAXSOCKADDR];
	} un;
	socklen_t	addrlen;

    if ( m_family == AF_INET )
    {
        struct sockaddr_in* addr = (struct sockaddr_in*)un.data;
        addrlen = sizeof(struct sockaddr_in);
        addr->sin_family = m_family;
        addr->sin_port = htons(port);
        addr->sin_addr.s_addr = ip?inet_addr(ip):htonl(INADDR_ANY);
        if ( bind(m_socketFd, (struct sockaddr*)&un.sa, addrlen) != 0 )
        {
          #if defined(WIN32) || (_WINDOWS)
            m_nErrno = WSAGetLastError();
          #else
            m_nErrno = errno;
          #endif
            SetError(m_nErrno);
            return false;
        }
    }
    else if ( m_family == AF_INET6 )
    {
        struct sockaddr_in6* addr = (struct sockaddr_in6*)un.data;
        addrlen = sizeof(struct sockaddr_in6);
        addr->sin6_family = m_family;
        addr->sin6_port = htons(port);
      #if defined(WIN32) || defined(_WINDOWS)
        m_nErrno = WSAENOTSOCK;       // Invalid argument windows不支持ipv6
		SetError(m_nErrno);
		return false;
      #else
        if ( ip == NULL )
            addr->sin6_addr = in6addr_any;
        else if ( inet_pton(m_family, ip, &addr->sin6_addr) < 0 )
        {
            m_nErrno = errno;
		    SetError(m_nErrno);
		    return false;
        }
      #endif
        if ( bind(m_socketFd, (struct sockaddr*)&un.sa, addrlen) != 0 )
        {
          #if defined(WIN32) || (_WINDOWS)
            m_nErrno = WSAGetLastError();
          #else
            m_nErrno = errno;
          #endif
            SetError(m_nErrno);
            return false;
        }
    }
    return true;
}

bool 
RSocket::Bind(const SockAddr* addr, socklen_t addrlen)
{
    if ( !IsValid() )
    {
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = WSAENOTSOCK;       // Invalid argument 
      #else
        m_nErrno = EBADF;      // Invalid argument 
      #endif
		SetError(m_nErrno);
        return false;
    }

    if ( addr == NULL || addrlen <= 0 )
    {
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = WSAEINVAL;       // Invalid argument 
      #else
        m_nErrno = EINVAL;      // Invalid argument 
      #endif
		SetError(m_nErrno);
        return false;
    }
    if ( bind(m_socketFd, addr, addrlen) != 0 )
    {
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = WSAGetLastError();
      #else
        m_nErrno = errno;
      #endif
        SetError(m_nErrno);
        return false;
    }
    return true;
}

bool 
RSocket::Bind(const RNetAddress& localAddr)
{
    if ( !IsValid() )
    {
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = WSAENOTSOCK;       // Invalid argument 
      #else
        m_nErrno = EBADF;      // Invalid argument 
      #endif
		SetError(m_nErrno);
        return false;
    }
    int n = localAddr.GetNumOfAddrinfo();
    const SockAddr* sin;
    size_t len;
    int i = 0;
    for ( ; i < n; i++)
    {
        sin = localAddr.GetSockAddr(i, NULL, NULL, NULL, &len);
        if ( bind(m_socketFd, sin, (socklen_t)len) == 0 )  //只要localAddr其中一个地址绑定成功即可
        {
            break;
        }
    }
    if ( i >= n )       //localAddr中地址均绑定失败
    {
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = WSAGetLastError();
      #else
        m_nErrno = errno;
      #endif
        SetError(m_nErrno);
    }
    return true;
}

bool 
RSocket::Close()
{
    if ( IsValid() && !IsClosed() )
    {
        int ret = CloseSocket(m_socketFd);
        if ( ret == SOCKET_ERROR )
        {
          #if defined(WIN32) || (_WINDOWS)
            m_nErrno = WSAGetLastError();
          #else
            m_nErrno = errno;
          #endif
            SetError(m_nErrno);
            return false;
        }
        m_totalBytesSent = 0;
        m_totalBytesReceived = 0;
        m_CloseTime = time(NULL);
        m_socketFd = INVALID_SOCKET;
        return true;
    }
    return true;
}

bool 
RSocket::IsClosed() const
{
    return m_CloseTime==0?false:true;
}

void 
RSocket::Shutdown(SHUTDOWN_TYPE type)
{
    shutdown(m_socketFd, type);
}

SHUTDOWN_TYPE 
RSocket::GetShutdownType() const
{
    return m_shutdownType;
}

int 
RSocket::GetSockName(RNetAddress& sockname)
{
    if ( !IsValid() )
        return -1;

	union 
	{
	  struct sockaddr	sa;
	  char				data[MAXSOCKADDR];
	} un;
	socklen_t	len;
    if ( m_family == AF_INET )
        len = sizeof(struct sockaddr_in);
    else if ( m_family == AF_INET6 )
        len = sizeof(struct sockaddr_in6);

	len = MAXSOCKADDR;
	if (getsockname(m_socketFd, (struct sockaddr*)un.data, &len) < 0)
	{
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = WSAGetLastError();
      #else
        m_nErrno = errno;
      #endif
        SetError(m_nErrno);
		return(-1);
    }

    sockname = RNetAddress(&un.sa, len);
    m_family = un.sa.sa_family;
	return m_family;
}

int 
RSocket::GetPeerName(RNetAddress& peername)
{
    if ( !IsValid() )
        return -1;
	union 
	{
	  struct sockaddr	sa;
	  char				data[MAXSOCKADDR];
	} un;
	socklen_t	len;
    if ( m_family == AF_INET )
        len = sizeof(struct sockaddr_in);
    else if ( m_family == AF_INET6 )
        len = sizeof(struct sockaddr_in6);

	len = MAXSOCKADDR;
	if (getpeername(m_socketFd, (struct sockaddr*)un.data, &len) < 0)
	{
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = WSAGetLastError();
      #else
        m_nErrno = errno;
      #endif
        SetError(m_nErrno);
		return(-1);
    }

    peername = RNetAddress(&un.sa, len);
    m_family = un.sa.sa_family;
	return m_family;
}

bool 
RSocket::Select(int myfdset, 
                int millisecond /*= -1*/,
                bool* readable /*= NULL*/,
                bool* writeable /*= NULL*/,
                bool* errable /*= NULL*/)
{
    if ( IsValid() == false )
        return false;

    fd_set *rset = NULL, *wset = NULL, *eset = NULL;
    fd_set theset;
    FD_ZERO(&theset);
    FD_SET(m_socketFd, &theset);
    if ( myfdset & FD_Readable )
    {
        rset = new fd_set;
        *rset = theset;
    }
    if ( myfdset & FD_Writeable )
    {
        wset = new fd_set;
        *wset = theset;
    }
    if ( myfdset & FD_Errable )
    {
        eset = new fd_set;
        *eset = theset;
    }

    struct timeval tval;
    if ( millisecond >= 0 )
    {
        tval.tv_sec = millisecond / 1000;               //秒
        tval.tv_usec = ( millisecond % 1000 ) * 1000;   //微秒
    }

    int ret = select((int)(m_socketFd + 1), rset, wset, eset, millisecond>=0?&tval:NULL);
    if ( ret == SOCKET_ERROR )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(WSAGetLastError());
        if ( m_nErrno == WSAEINTR )     //中断信号
      #else
        SetError(errno);
        if ( m_nErrno == EINTR )        //中断信号
      #endif
        {
            return false;
        }
    }
    else if ( ret == 0 )    //超时
    {
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = ERROR_TIMEOUT;
      #else
        m_nErrno = ETIMEDOUT;
      #endif
        SetError(m_nErrno);
    }
    else                    //可读写或异常
    {
        if ( rset != NULL && readable != NULL && FD_ISSET(m_socketFd, rset) )
        {
            *readable = true;
        }
        if ( wset != NULL && writeable != NULL && FD_ISSET(m_socketFd, wset) )
        {
            *writeable = true;
        }
        if ( eset != NULL && errable != NULL && FD_ISSET(m_socketFd, eset) )
        {
            *errable = true;
        }
        ResetError();
    }
    if ( rset != NULL )
        delete rset;
    if ( wset != NULL )
        delete wset;
    if ( eset != NULL )
        delete eset;

    if ( ret <= 0 )
        return false;
    return true;
}

void 
RSocket::AddBytesSent(unsigned long sentBytes)
{
    m_totalBytesSent += sentBytes;
}

unsigned long 
RSocket::GetBytesSent(bool clear /*= false*/)
{
    unsigned long tmp = m_totalBytesSent;
    if ( clear == true )
        m_totalBytesSent = 0;
    return tmp;
}

void 
RSocket::AddBytesRecved(unsigned long recvedBytes)
{
    m_totalBytesReceived += recvedBytes;
}

unsigned long 
RSocket::GetByteRecved(bool clear /*= false*/)
{
    unsigned long tmp = m_totalBytesReceived;
    if ( clear == true )
        m_totalBytesReceived = 0;
    return tmp;
}

time_t 
RSocket::CreatTime() const
{
	return m_CreaTime;
}

time_t 
RSocket::CloseTime() const
{
	return m_CloseTime;
}

void 
RSocket::SetError(int syserr)
{
    m_nErrno = syserr;
}

bool 
RSocket::IsError() const
{
	return m_nErrno==0?false:true;
}

const RString& 
RSocket::ReportError()
{
    if ( !IsError() )
    {
        m_lasterror = "No Error!";
        return m_lasterror;
    }
	char ctmp[256];
	RString strerr;
  #if defined (WIN32) || defined (_WINDOWS)
    strerr = GetSockError(m_nErrno);
    if ( strerr.empty() )
    {
    	strerror_s(ctmp, sizeof(ctmp), m_nErrno);
    	strerr = ctmp;
    }
  #else
    strerror_r(m_nErrno, ctmp, sizeof(ctmp));
	strerr = ctmp;
  #endif
  #if defined (_DEBUG)
	m_lasterror.Format("socket error:%s, %d, %s", __FILE__, __LINE__, strerr.c_str());
  #else
    m_lasterror.Format("socket error:%s", strerr.c_str());
  #endif
	return m_lasterror;
}

void 
RSocket::ResetError()
{
	m_nErrno = 0;
	m_lasterror = RString();
}

bool 
RSocket::SetBlockType(bool bBlock)
{
#if defined(WIN32) || defined(_WINDOWS)
	unsigned long ulong = bBlock ? false : true;
	int n = ioctlsocket(m_socketFd, FIONBIO, &ulong);
	if (n != 0)
	{
		return false;
	}
	m_blocktype = bBlock;
	return true;
#else
    int flags = 0;
	if (bBlock)
	{
	    flags = fcntl(m_socketFd,F_GETFL,0);
		if (fcntl(m_socketFd, F_SETFL, flags&(!O_NONBLOCK)) == -1)
		{
			return false;
		}
	}
	else
	{
	    flags = fcntl(m_socketFd,F_GETFL,0);
		if (fcntl(m_socketFd, F_SETFL, flags|O_NONBLOCK) == -1)
		{
			return false;
		}
	}
	m_blocktype = bBlock;
	return true;
#endif
}

bool 
RSocket::IsBlockType() const
{ 
    return m_blocktype;
}

int 
RSocket::SockfdToFamily()
{
    if ( !IsValid() )
        return -1;

	union 
	{
	  struct sockaddr	sa;
	  char				data[MAXSOCKADDR];
	} un;
	socklen_t	len;

	len = MAXSOCKADDR;
	if (getsockname(m_socketFd, (struct sockaddr*)un.data, &len) < 0)
	{
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = WSAGetLastError();
      #else
        m_nErrno = errno;
      #endif
		SetError(m_nErrno);
		return(-1);
    }
    m_family = un.sa.sa_family;
	return m_family;
}
