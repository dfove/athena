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
    m_blocktype = blocktype;  //acceptĬ�ϵľ�Ϊbolck socket
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
        //�����û�����µ�socket��֮ǰ���Ѿ��رգ������´����µ�socket
        //���ߣ������֮ǰ��connectʧ�ܣ�����Ҫ�ر�socket�����´����µ�socket��������
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
            break;  //���ӳɹ������ڷ�����socket����ô���ܳɹ���һ�㶼�Ǳ�����ַ����
        }

        //����Ϊ����ʧ�ܵĴ����ַ�Ϊ�����ͷ�����ģʽ�����
      #if defined(WIN32) || (_WINDOWS)
        SetError(WSAGetLastError());
      #else
        SetError(errno);
      #endif
        if ( IsBlockType() == true )    //���õ�����socket
        {
            Close();                    //���connectʧ�ܣ���������´����µ�socket�����´�����
        }
        else                            //������socket����������ʾ�������ӣ������ж�ָ��ʱ��֮���Ƿ�������
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
             * �����ָ����ʱ���ڿɶ�д,˵���п��������ϣ�Ҫô���ǳ����쳣��ʱ
             */
            if ( Select(FD_Readable|FD_Writeable, waitMilliseconds) == true )
            {
                socklen_t len = sizeof(m_nErrno);
                if ( getsockopt(m_socketFd, SOL_SOCKET, SO_ERROR, (char*)&m_nErrno, &len) == SOCKET_ERROR )
                {//���getsockoptʧ��
                  #if defined(WIN32) || (_WINDOWS)
                    SetError(WSAGetLastError());
                  #else
                    SetError(errno);
                  #endif
                    Close();
                }
                else
                {
                    if ( m_nErrno )     //�������(������connect�ھ���Ԥ��ʱ��֮��)�д���
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
            else        //��ʱ��select����
            {
                Close();
            }
        }   //������
    }       //remoteAddr�����е�ַѭ��
    if ( !IsValid() )   //���е�ַ��connectʧ��
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
    if ( !IsValid() )   //�����µ�socket����bind�ɹ�
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
        if ( port != -1 )  //�����ָ���˿�,���԰�
        {
            if ( Bind(port, lpszIP) == false )
            {
              #if defined(WIN32) || (_WINDOWS)
                if (m_nErrno != WSAEINVAL)      //��ʧ���Ҳ���"�Ѱ�"�Ĵ���,��ر�socket,���ܵ���listen
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
    if ( IsBlockType() == true )        //����
    {
        fd = accept(m_socketFd, (struct sockaddr*)un.data, &len);
    }
    else        //������
    {
        if ( Select(FD_Readable, waitMilliseconds) == true )
            fd = accept(m_socketFd, (struct sockaddr*)un.data, &len);
        else
        {
          #if defined(WIN32) || (_WINDOWS)
            if ( m_nErrno == WSAEINTR )     //�ж��ź�
          #else
            if ( m_nErrno == EINTR )        //�ж��ź�
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
    connfd.m_blocktype = true;  //acceptĬ�ϵľ�Ϊbolck socket
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
        if ( waitMilliseconds >= 0  && Select(FD_Writeable, waitMilliseconds) == false )   //��ָ��ʱ�����Ƿ��д
        {
          #if defined(WIN32) || (_WINDOWS)
            if ( m_nErrno == WSAEINTR )     //�ж��ź�
          #else
            if ( m_nErrno == EINTR )        //�ж��ź�
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
            if ( m_nErrno == WSAEINTR )     //�ж��ź�
          #else
            SetError(errno);
            if ( m_nErrno == EINTR )        //�ж��ź�
          #endif
            {
                continue;
            }
            if ( IsBlockType() == false )    //������ģʽ
            {
              #if defined(WIN32) || (_WINDOWS)
                if ( m_nErrno == WSAEWOULDBLOCK )     //�ж��ź�
              #else
                if ( m_nErrno == EWOULDBLOCK )        //�ж��ź�
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
        if ( waitMilliseconds >= 0  && Select(FD_Readable, waitMilliseconds) == false )   //��ָ��ʱ�����Ƿ�ɶ�
        {
          #if defined(WIN32) || (_WINDOWS)
            if ( m_nErrno == WSAEINTR )     //�ж��ź�
          #else
            if ( m_nErrno == EINTR )        //�ж��ź�
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
            if ( m_nErrno == WSAEINTR )     //�ж��ź�
          #else
            SetError(errno);
            if ( m_nErrno == EINTR )        //�ж��ź�
          #endif
            {
                continue;
            }
            if ( IsBlockType() == false )    //������ģʽ
            {
              #if defined(WIN32) || (_WINDOWS)
                if ( m_nErrno == WSAEWOULDBLOCK )     //�ж��ź�
              #else
                if ( m_nErrno == ECONNRESET )         //ECONNRESET:Connection reset by peer 
                {
                    Close();
                    return E_Breaken;
                }
                if ( m_nErrno == EWOULDBLOCK )        //�ж��ź�
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
