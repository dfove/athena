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

#include <RTcpSocket.hpp>
#include <RTime.hpp>

using namespace athena;

RTcpSocket::RTcpSocket(MyIpType addressFamily /*= IPV4*/, int protocol /*= 0*/, bool bAutoClose /*= true*/)
           :RSocket(addressFamily==IPV4?AF_INET:AF_INET6, SOCK_STREAM, protocol, bAutoClose)
           ,m_isListening(false)
           ,m_isConnected(false)
{
    
}

RTcpSocket::RTcpSocket(SOCKET sockfd, 
                       bool bBlock /*= true*/, 
                       bool bAutoClose /*= true*/)
           :RSocket(sockfd, SOCK_STREAM, bBlock, bAutoClose)
           ,m_isListening(false)
           ,m_isConnected(false)
{
}

RTcpSocket::RTcpSocket(const RNetAddress & localAddr, bool bAutoClose /*= true*/)
           :RSocket(localAddr, SOCK_STREAM, 0, bAutoClose)
           ,m_isListening(false)
           ,m_isConnected(false)
{
    Bind(localAddr);
}

RTcpSocket::~RTcpSocket()
{

}

int 
RTcpSocket::AttachSocketFd(SOCKET fd, 
                           bool blocktype /* = true */,
                           bool bConnected /* = true */,
                           MyIpType addressFamily /* = IPV4 */, 
                           int protocol /* = 0 */,
                           bool autoClose /* = true */)
{
    int iOldSocketFd = m_socketFd;
    m_socketFd = fd;
    m_family = IPV4?AF_INET:AF_INET6;
    m_socketType = SOCK_STREAM;
    m_protocol = m_protocol;
    SetBlockType(blocktype);
    m_blocktype = blocktype;  //accept默认的均为bolck socket
    m_bAutoClose = autoClose;
    m_shutdownType = SHUTDOWN_NONE;
    m_totalBytesSent = 0;
    m_totalBytesReceived = 0;
    m_CreaTime = time(NULL);
    m_CloseTime = 0;
    m_nErrno = 0;
    m_lasterror.clear();
    m_isListening = !bConnected;
    m_isConnected = bConnected;
    return iOldSocketFd;
}

bool 
RTcpSocket::Connect(const RString& host, int nPort, unsigned waitMilliseconds /*= 10*1000*/)
{
    if ( m_isConnected == true )
    {
        Close();
        m_isListening = false;
        m_isConnected = false;
    }
    RNetAddress netaddr(host, nPort, AF_UNSPEC, SOCK_STREAM);
    return Connect(netaddr, waitMilliseconds);
}

bool 
RTcpSocket::Connect(const RNetAddress& remoteAddr, unsigned waitMilliseconds /*= 10*1000*/)
{
    if ( m_isConnected == true )
    {
        Close();
        m_isListening = false;
        m_isConnected = false;
    }
    int n = remoteAddr.GetNumOfAddrinfo();
    const SockAddr* saddr;
    int fa = -1, socktype = -1, protocol = -1;
    size_t saddrlen;
    int i = 0;
    for ( ; i < n; i++ )
    {
        saddr = remoteAddr.GetSockAddr(i, &fa, &socktype, &protocol, &saddrlen);
        //如果还没创建新的socket或之前的已经关闭，则重新创建新的socket
        //或者，如果是之前的connect失败，则需要关闭socket再重新创建新的socket用于连接
        if ( socktype != SOCK_STREAM )
            continue;
        if ( IsValid() == false && 
             Create(fa, socktype, protocol) == SOCKET_ERROR )
        {
          #if defined(WIN32) || (_WINDOWS)
            SetError(WSAGetLastError());
          #else
            SetError(errno);
          #endif
            continue;
        }
        if ( connect(m_socketFd, saddr, (int)saddrlen) != SOCKET_ERROR )
        {
            ResetError();
            break;  //连接成功，对于非阻塞socket，这么快能成功，一般都是本机地址连接
        }

        //以下为连接失败的处理，又分为阻塞和非阻塞模式的情况
      #if defined(WIN32) || (_WINDOWS)
        SetError(WSAGetLastError());
      #else
        SetError(errno);
      #endif
        if ( IsBlockType() == true )    //常用的阻塞socket
        {
            Close();                    //如果connect失败，则必须重新创建新的socket用与下次连接
        }
        else                            //非阻塞socket，如果错误表示正在连接，则需判断指定时间之后是否连接上
        {
          #if defined(WIN32) || (_WINDOWS)
            if ( m_nErrno != WSAEWOULDBLOCK )
          #else
            if ( m_nErrno != EINPROGRESS )
          #endif
            {
                Close();
                continue;
            }
            /*
             * 如果在指定的时间内可读写,说明有可能连接上；要么就是出现异常或超时
             */
            if ( Select(FD_Readable|FD_Writeable, waitMilliseconds) == true )
            {
                socklen_t len = sizeof(m_nErrno);
                if ( getsockopt(m_socketFd, SOL_SOCKET, SO_ERROR, (char*)&m_nErrno, &len) == SOCKET_ERROR )
                {//如果getsockopt失败
                  #if defined(WIN32) || (_WINDOWS)
                    SetError(WSAGetLastError());
                  #else
                    SetError(errno);
                  #endif
                    Close();
                }
                else
                {
                    if ( m_nErrno )     //如果连接(非阻塞connect在经过预订时间之后)有错误
                    {
                        Close();
                        SetError(m_nErrno);
                    }
                    else
                    {
                        ResetError();
                        break;
                    }
                }
            }
            else        //超时、select出错
            {
                Close();
            }
        }   //非阻塞
    }       //remoteAddr的所有地址循环
    if ( !IsValid() )   //所有地址都connect失败
    {
        return false;
    }
    m_isConnected = true;
    return true;
}

bool 
RTcpSocket::IsConnected() const
{
    return m_isConnected;
}

bool 
RTcpSocket::Close()
{
    m_isConnected = false;
    return RSocket::Close();
}

int 
RTcpSocket::Listen(int maxconn,
                   int port /*= -1*/,
                   const char* lpszIP /*= NULL*/)
{
    if ( !IsValid() )   //创建新的socket，并bind成功
    {
        if ( port == -1 )
        {
          #if defined(WIN32) || (_WINDOWS)
            SetError(WSAEINVAL);       // Invalid argument 
          #else
            SetError(EINVAL);      // Invalid argument 
          #endif
            return E_Port;
        }
        RNetAddress *localaddr;
        if ( lpszIP == NULL )
        {
            localaddr = new RNetAddress(port, AF_UNSPEC, SOCK_STREAM, true);
        }
        else
        {
            RString strIP(lpszIP);
            if ( strIP.IsValidIPAddress() == false )
                return E_Host;
            localaddr = new RNetAddress(lpszIP, port, AI_PASSIVE, AF_UNSPEC, SOCK_STREAM, 0);
        }
        int n = localaddr->GetNumOfAddrinfo();
        const SockAddr* saddr;
        int fa = 0, socktype = 0, protocol = 0;
        size_t saddrlen;
        for ( int i = 0; i < n; i++ )
        {
            saddr = localaddr->GetSockAddr(i, &fa, &socktype, &protocol, &saddrlen);
            if ( Create(fa, socktype, protocol) == SOCKET_ERROR )
                continue;
            if ( Bind(saddr, (socklen_t)saddrlen) == true )
                break;
            Close();
        }
        if ( localaddr )
            delete localaddr;
    }
    else
    {
        if ( port != -1 )  //如果已指定端口,则尝试绑定
        {
            if ( Bind(port, lpszIP) == false )
            {
              #if defined(WIN32) || (_WINDOWS)
                if (m_nErrno != WSAEINVAL)      //绑定失败且不是"已绑定"的错误,则关闭socket,不能调用listen
              #else
                if ( m_nErrno != EINVAL )
              #endif
                {
                    Close();
                }
                return E_Bind;
            }
        }
    }

    if ( IsValid() && listen(m_socketFd, maxconn) == SOCKET_ERROR )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(WSAGetLastError());
      #else
        SetError(errno);
      #endif
      return E_Listen;
    }
    m_isListening = true;
    return E_Success;
}

SOCKET 
RTcpSocket::Accept(const unsigned uMilliseconds/*=10*/)
{
    struct sockaddr_in sin;
    socklen_t slLen;
    SOCKET sLink = -1;
    memset(&sin, 0, sizeof(sin));
    slLen =	sizeof(sin);
    if ( Select(FD_Readable, uMilliseconds) == true )
    {
        sLink =	accept(	m_socketFd,(struct sockaddr*)&sin, &slLen);
    }
    else
    {
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = ERROR_TIMEOUT;
      #else
        m_nErrno = ETIMEDOUT;
      #endif
        SetError(m_nErrno);
    }
    return sLink;
}

SOCKET 
RTcpSocket::Accept(RString& sIP, unsigned& uPort, const unsigned uMilliseconds /*= 10*/)
{
    struct sockaddr_in sin;
    socklen_t slLen;
    SOCKET sLink = -1;
    memset(&sin, 0, sizeof(sin));
    slLen =	sizeof(sin);
    if( Select(FD_Readable, uMilliseconds) == true )
    {
        sLink =	accept(	m_socketFd, (sockaddr*)&sin, &slLen);
        RNetAddress tmp((sockaddr*)&sin, slLen);
        sIP = tmp.GetIP((sockaddr*)&sin);
        uPort = tmp.GetPort((sockaddr*)&sin);
    }
    else
    {
      #if defined(WIN32) || (_WINDOWS)
        m_nErrno = ERROR_TIMEOUT;
      #else
        m_nErrno = ETIMEDOUT;
      #endif
        SetError(m_nErrno);
    }
    return sLink;
}

int 
RTcpSocket::Accept(RTcpSocket& connfd, unsigned waitMilliseconds /*= 100*/)
{
    RString ip;
    int port;
    return Accept(connfd, ip, port, waitMilliseconds);
}

int 
RTcpSocket::Accept(RTcpSocket& connfd, RString& ip, int& port, unsigned waitMilliseconds /*= 100*/)
{
    if ( !IsValid() )
    {
        return E_Sockfd;
    }

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

    SOCKET fd;     
    if ( IsBlockType() == true )        //阻塞
    {
        fd = accept(m_socketFd, (struct sockaddr*)un.data, &len);
    }
    else        //非阻塞
    {
        if ( Select(FD_Readable, waitMilliseconds) == true )
            fd = accept(m_socketFd, (struct sockaddr*)un.data, &len);
        else
        {
          #if defined(WIN32) || (_WINDOWS)
            if ( m_nErrno == WSAEINTR )     //中断信号
          #else
            if ( m_nErrno == EINTR )        //中断信号
          #endif
            {
                return E_INTERRUPT;
            }
            return E_Timeout;
        }
    }
    if ( fd == SOCKET_ERROR )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(WSAGetLastError());
      #else
        SetError(errno);
      #endif
        return E_Accept;
    }
    //connfd.SetSocketFd(fd, m_socketType, m_protocol, m_blocktype, m_bAutoClose);
    connfd.m_socketFd = fd;
    connfd.m_family = m_family;
    connfd.m_socketType = m_socketType;
    connfd.m_protocol = m_protocol;
    connfd.m_blocktype = true;  //accept默认的均为bolck socket
    connfd.m_bAutoClose = m_bAutoClose;
    connfd.m_shutdownType = SHUTDOWN_NONE;
    connfd.m_totalBytesSent = 0;
    connfd.m_totalBytesReceived = 0;
    connfd.m_CreaTime = time(NULL);
    connfd.m_CloseTime = 0;
    connfd.m_nErrno = 0;
    connfd.m_lasterror.clear();
    connfd.m_isListening = false;
    connfd.m_isConnected = true;
    RNetAddress tmp(&un.sa, len);
    ip = tmp.GetIP(&un.sa);
    port = tmp.GetPort(&un.sa);

    return E_Success;
}

int 
RTcpSocket::SendOnce(const char* pData, int nbytes)
{
    if ( !IsValid() || m_isListening == true )
    {
        return SOCKET_ERROR;
    }
    int ret = send(m_socketFd, pData, nbytes, 0);
    if ( ret == SOCKET_ERROR )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(WSAGetLastError());
      #else
        SetError(errno);
      #endif
    }
    else
        AddBytesSent(ret);
    return ret;
}

int 
RTcpSocket::SendOnce(const RString& buf)
{
    return SendOnce(buf.c_str(), (int)buf.size());
}

int 
RTcpSocket::SendOnce(const RStream& stream)
{
    return SendOnce(stream.Data(), (int)stream.Size());
}

int 
RTcpSocket::RecvOnce(char* pData, int nbytes)
{
    if ( !IsValid() || m_isListening == true )
        return SOCKET_ERROR;
    int ret = recv(m_socketFd, pData, nbytes, 0);
    if ( ret == SOCKET_ERROR )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(WSAGetLastError());
      #else
        SetError(errno);
      #endif
    }
    else
        AddBytesRecved(ret);
    return ret;
}

int 
RTcpSocket::RecvOnce(RString& buf, int nbytes)
{
    char* tmpbuf = new char[nbytes+1];
    memset(tmpbuf, 0, nbytes+1);
    int ret = RecvOnce(tmpbuf, nbytes);
    if ( ret != SOCKET_ERROR )
        buf = RString(tmpbuf);
    delete[] tmpbuf;
    return ret;
}

int 
RTcpSocket::RecvOnce(RStream& stream, int nbytes)
{
    char* tmpbuf = new char[nbytes];
    memset(tmpbuf, 0, nbytes);
    int ret = RecvOnce(tmpbuf, nbytes);
    if ( ret != SOCKET_ERROR )
        stream = RStream(tmpbuf, nbytes);
    delete[] tmpbuf;
    return ret;
}

int 
RTcpSocket::Sendn(const char* pData, int nbytes, int waitMilliseconds /*= -1*/)
{
    if ( !IsValid() || m_isListening == true )
        return E_Sockfd;

    int sentLen = 0;
    while( sentLen < nbytes )
    {
        if ( waitMilliseconds >= 0  && Select(FD_Writeable, waitMilliseconds) == false )   //在指定时间内是否可写
        {
          #if defined(WIN32) || (_WINDOWS)
            if ( m_nErrno == WSAEINTR )     //中断信号
          #else
            if ( m_nErrno == EINTR )        //中断信号
          #endif
            {
                return E_INTERRUPT;
            }
            return E_Timeout;
        }
        int ret = send(m_socketFd, pData + sentLen, nbytes - sentLen, 0);
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
                continue;
            }
            if ( IsBlockType() == false )    //非阻塞模式
            {
              #if defined(WIN32) || (_WINDOWS)
                if ( m_nErrno == WSAEWOULDBLOCK )     //中断信号
              #else
                if ( m_nErrno == EWOULDBLOCK )        //中断信号
              #endif
                {
                    return E_WOULDBLOCK;
                }
            }
            return E_Send;
        }
        else
        {
            sentLen +=  ret;
        }
    }
    AddBytesSent(nbytes);
    return E_Success;
}

int 
RTcpSocket::Sendn(const RString& buf, int waitMilliseconds /*= -1*/)
{
    return Sendn(buf.c_str(), (int)buf.size(), waitMilliseconds);
}

int 
RTcpSocket::Sendn(const RStream& stream, int waitMilliseconds /*= -1*/)
{
    return Sendn(stream.Data(), (int)stream.Size(), waitMilliseconds);
}

int 
RTcpSocket::Recvn(char* pData, int nbytes, int waitMilliseconds /*= -1*/)
{
    if ( !IsValid() || m_isListening == true )
        return E_Sockfd;

    int recvLen = 0;
    while ( recvLen < nbytes )
    {
        errno = 0;
        if ( waitMilliseconds >= 0  && Select(FD_Readable, waitMilliseconds) == false )   //在指定时间内是否可读
        {
          #if defined(WIN32) || (_WINDOWS)
            if ( m_nErrno == WSAEINTR )     //中断信号
          #else
            if ( m_nErrno == EINTR )        //中断信号
          #endif
            {
                return E_INTERRUPT;
            }
            return E_Timeout;
        }
        int ret = recv(m_socketFd, pData + recvLen, nbytes - recvLen, 0);
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
                continue;
            }
            if ( IsBlockType() == false )    //非阻塞模式
            {
              #if defined(WIN32) || (_WINDOWS)
                if ( m_nErrno == WSAEWOULDBLOCK )     //中断信号
              #else
                if ( m_nErrno == ECONNRESET )         //ECONNRESET:Connection reset by peer 
                {
                    Close();
                    return E_Breaken;
                }
                if ( m_nErrno == EWOULDBLOCK )        //中断信号
              #endif
                {
                    return E_WOULDBLOCK;
                }
            }
            Close();
            return E_Recv;
        }
        else if ( ret == 0 )    // perr close the socket
        {
            Close();
            return E_Breaken;
        }
        else
        {
            recvLen += ret;
        }
    }
    AddBytesRecved(nbytes);
    return E_Success;
}

int 
RTcpSocket::Recvn(RString& buf, int nbytes, int waitMilliseconds /*= -1*/)
{
    char* tmpbuf = new char[nbytes+1];
    memset(tmpbuf, 0, nbytes+1);
    int ret = Recvn(tmpbuf, nbytes, waitMilliseconds);
    if ( ret == E_Success )
        buf = RString(tmpbuf);
    delete[] tmpbuf;
    return ret;
}

int 
RTcpSocket::Recvn(RStream& stream, int nbytes, int waitMilliseconds /*= -1*/)
{
    char* tmpbuf = new char[nbytes];
    memset(tmpbuf, 0, nbytes);
    int ret = Recvn(tmpbuf, nbytes, waitMilliseconds);
    if ( ret == E_Success )
        stream.Assign(tmpbuf, nbytes);
    delete[] tmpbuf;
    return ret;
}
