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

#include <RUdpSocket.hpp>

using namespace athena;

RUdpSocket::RUdpSocket(int addrFamily /*= AF_INET*/, bool bAutoClose /*= true*/)
		   :RSocket(addrFamily, SOCK_DGRAM, 0, bAutoClose)
{
	
}

RUdpSocket::RUdpSocket(const RNetAddress& localAddress, bool bAutoClose /*= true*/)
		   :RSocket(localAddress, SOCK_DGRAM, bAutoClose)
{
	
}

RUdpSocket::~RUdpSocket()
{
	
}

bool 
RUdpSocket::Connect(const RString& host, int nPort)
{
    if ( m_isConnected == true )
        return true;
    RNetAddress netaddr(host, nPort, AF_UNSPEC, SOCK_DGRAM);
    return Connect(netaddr);
}

bool 
RUdpSocket::Connect(const RNetAddress& remoteAddr)
{
    if ( m_isConnected == true )
        return true;

    int n = remoteAddr.GetNumOfAddrinfo();
    const SockAddr* saddr;
    int fa = 0, socktype = 0, protocol = 0;
    size_t saddrlen;
    int i = 0;
    for ( ; i < n; i++ )
    {
        saddr = remoteAddr.GetSockAddr(i, &fa, &socktype, &protocol, &saddrlen);
        if ( socktype != SOCK_DGRAM )
            continue;
        if ( IsValid() == false &&  Create(fa, socktype, protocol) == SOCKET_ERROR )
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
            break;
        }
        Close();
    }
    if ( IsValid() == false )
        return false;
	return true;
}

bool 
RUdpSocket::IsConnected() const
{
    return m_isConnected;
}

int 
RUdpSocket::Send(const char* pData, int nSize)
{
    if ( IsConnected() == false )
        return false;

    int ret = send(m_socketFd, pData, nSize, 0);
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
RUdpSocket::SendTo(const char* buf, int nbytes, 
	               const char* ip, int port, 
	               int waitMilliseconds /*= -1*/)
{
    if ( IsValid() == false )
        return SOCKET_ERROR;
 
    if ( buf == NULL || nbytes < 0 || ip == NULL || port <= 0 )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(WSAEINVAL);       // Invalid argument 
      #else
        SetError(EINVAL);      // Invalid argument 
      #endif
        return SOCKET_ERROR;
    }

    RNetAddress remoteAddr(ip, port, m_family, SOCK_DGRAM);
    return SendTo(buf, nbytes, remoteAddr, waitMilliseconds);
}

int 
RUdpSocket::SendTo(const char* buf, int nbytes, 
	               const RNetAddress& remoteAddr,
	               int waitMilliseconds /*= -1*/)
{
    if ( IsValid() == false )
        return SOCKET_ERROR;

    if ( Select(FD_Writeable, waitMilliseconds) == false )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(ERROR_TIMEOUT);
      #else
        SetError(ETIMEDOUT);
        m_nErrno = ETIMEDOUT;
      #endif
      return SOCKET_ERROR;
    }

    int n = remoteAddr.GetNumOfAddrinfo();
    if ( n <= 0 )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(WSAEINVAL);       // Invalid argument 
      #else
        SetError(EINVAL);      // Invalid argument 
      #endif
        return SOCKET_ERROR;
    }

    const SockAddr* saddr;
    size_t saddrlen;
    int i = 0;
    for ( ; i < n; i++ )
    {
        saddr = remoteAddr.GetSockAddr(i, NULL, NULL, NULL, &saddrlen);
        int ret = sendto(m_socketFd, buf, nbytes, 0, saddr, (int)saddrlen);
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
                i--;
                continue;
            }
        }
        else
        {
            ResetError();
            return ret;
        }
    }
    return SOCKET_ERROR;
}

int 
RUdpSocket::SendTo(const RString& buf, 
                   const RNetAddress& remoteAddr,
                   int waitMilliseconds /*= -1*/)
{
    return SendTo(buf.c_str(), (int)buf.size(), remoteAddr, waitMilliseconds);
}

int 
RUdpSocket::SendTo(const RStream& stream, 
                   const RNetAddress& remoteAddr,
                   int waitMilliseconds /*= -1*/)
{
    return SendTo(stream.Data(), (int)stream.Size(), remoteAddr, waitMilliseconds);
}

int 
RUdpSocket::RecvFrom(char* buf, int nbytes,
	                 char* ip /*= NULL*/, int* port /*= NULL*/,
	                 int waitMilliseconds /*= -1*/)
{
    if ( IsValid() == false )
        return SOCKET_ERROR;
 
    if ( buf == NULL || nbytes < 0 )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(WSAEINVAL);       // Invalid argument 
      #else
        SetError(EINVAL);      // Invalid argument 
      #endif
        return SOCKET_ERROR;
    }

    if ( Select(FD_Readable, waitMilliseconds) == false )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(ERROR_TIMEOUT);
      #else
        SetError(ETIMEDOUT);
      #endif
      return SOCKET_ERROR;
    }

	union 
	{
	  struct sockaddr	sa;
	  char				data[MAXSOCKADDR];
	} un;
	socklen_t	len;

    if ( m_family == AF_INET )
    {
        len = sizeof(struct sockaddr_in);
    }
    else if ( m_family == AF_INET6 )
    {
        len = sizeof(struct sockaddr_in6);
    }
    else
        return SOCKET_ERROR;

    int ret = recvfrom(m_socketFd, buf, nbytes, 0, (SockAddr*)un.data, &len);
    if ( ret == SOCKET_ERROR )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(WSAGetLastError());
      #else
        SetError(errno);
      #endif
      return SOCKET_ERROR;
    }
    RNetAddress remote(&un.sa, len);
    if ( ip != NULL )
    {
        RString strip = remote.GetIP(&un.sa);
        memcpy(ip, strip.c_str(), strip.size());
    }
    if ( port != NULL )
    {
        *port = remote.GetPort(&un.sa);
    }

    return ret;
}

int 
RUdpSocket::RecvFrom(char* buf, int nbytes, 
	                 RNetAddress& remoteAddr, 
	                 int waitMilliseconds /*= -1*/)
{
    if ( IsValid() == false )
        return SOCKET_ERROR;

    if ( Select(FD_Readable, waitMilliseconds) == false )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(ERROR_TIMEOUT);
      #else
        SetError(ETIMEDOUT);
        m_nErrno = ETIMEDOUT;
      #endif
      return SOCKET_ERROR;
    }

    char hostname[NI_MAXHOST];
    int port = -1;
    int ret = RecvFrom(buf, nbytes, hostname, &port, waitMilliseconds);
    remoteAddr = RNetAddress(hostname, port, m_family, SOCK_DGRAM);

    return ret;
}

int 
RUdpSocket::RecvFrom(RString& buf, int nbytes,
                     RNetAddress& remoteAddr,
                     int waitMilliseconds /*= -1*/)
{
    char tempbuf[MAXBUFFSIZE];
    memset(tempbuf, 0, MAXBUFFSIZE);
    int ret = RecvFrom(tempbuf, MAXBUFFSIZE, remoteAddr, waitMilliseconds);
    if ( ret != SOCKET_ERROR )
        buf = RString(tempbuf);
    return ret;
}

int 
RUdpSocket::RecvFrom(RStream& stream, int nbytes,
                     RNetAddress& remoteAddr,
                     int waitMilliseconds /*= -1*/)
{
    char tempbuf[MAXBUFFSIZE];
    memset(tempbuf, 0, MAXBUFFSIZE);
    int ret = RecvFrom(tempbuf, MAXBUFFSIZE, remoteAddr, waitMilliseconds);
    if ( ret != SOCKET_ERROR )
        stream = RStream(tempbuf, ret);
    return ret;
}
