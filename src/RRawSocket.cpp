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

#include <RRawSocket.hpp>

using namespace athena;

RRawSocket::RRawSocket(int addressFamily, 
                       int protocol, 
                       bool bAutoClose /*= true*/)
           :RSocket(addressFamily, SOCK_RAW, protocol, bAutoClose)
{
    
}

RRawSocket::~RRawSocket()
{

}

int 
RRawSocket::SendTo(const char* buf, int nbytes, const char* ip)
{
    if ( IsValid() == false )
        return SOCKET_ERROR;
 
    if ( buf == NULL || nbytes < 0 || ip == NULL )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(WSAEINVAL);       // Invalid argument 
      #else
        SetError(EINVAL);      // Invalid argument 
      #endif
        return SOCKET_ERROR;
    }

    RNetAddress remoteAddr(ip, 0, m_family, SOCK_RAW);
    return SendTo(buf, nbytes, remoteAddr);
}

int 
RRawSocket::SendTo(const char* buf, int nbytes, const RNetAddress& remoteAddr)
{
    if ( IsValid() == false )
        return SOCKET_ERROR;

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
RRawSocket::SendTo(const RString& str, const RNetAddress& remoteAddr)
{
    return SendTo(str.c_str(), (int)str.size(), remoteAddr);
}

int 
RRawSocket::SendTo(const RStream& stream, const RNetAddress& remoteAddr)
{
    return SendTo(stream.Data(), (int)stream.Size(), remoteAddr);
}

int 
RRawSocket::RecvFrom(char* buf, int nbytes, char* ip)
{
    if ( IsValid() == false )
        return SOCKET_ERROR;
 
    if ( buf == NULL || nbytes < 0 || ip == NULL )
    {
      #if defined(WIN32) || (_WINDOWS)
        SetError(WSAEINVAL);       // Invalid argument 
      #else
        SetError(EINVAL);      // Invalid argument 
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

    int ret = recvfrom(m_socketFd, buf, nbytes, 0, &un.sa, &len);
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
    return ret;
}

int 
RRawSocket::RecvFrom(char* buf, int nbytes, RNetAddress& remoteAddr)
{
    if ( IsValid() == false )
        return SOCKET_ERROR;

    char hostname[NI_MAXHOST];
    int ret = RecvFrom(buf, nbytes, hostname);
    remoteAddr = RNetAddress(hostname, 0, m_family, SOCK_RAW);

    return ret;
}

int 
RRawSocket::RecvFrom(RString& str, RNetAddress& remoteAddr)
{
    char tempbuf[MAXBUFFSIZE];
    memset(tempbuf, 0, MAXBUFFSIZE);
    int ret = RecvFrom(tempbuf, MAXBUFFSIZE, remoteAddr);
    if ( ret != SOCKET_ERROR )
        str = RString(tempbuf);
    return ret;
}

int 
RRawSocket::RecvFrom(RStream& stream, RNetAddress& remoteAddr)
{
    char tempbuf[MAXBUFFSIZE];
    memset(tempbuf, 0, MAXBUFFSIZE);
    int ret = RecvFrom(tempbuf, MAXBUFFSIZE, remoteAddr);
    if ( ret != SOCKET_ERROR )
        stream = RStream(tempbuf, ret);
    return ret;
}
