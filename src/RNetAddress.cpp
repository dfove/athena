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

#include <RNetAddress.hpp>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace athena;

RNetAddress::RNetAddress(int family /*= AF_UNSPEC*/, int sockType /*= 0*/)
			:m_family(family)
			,m_sockType(sockType)
			,m_protocol(0)
            ,m_isvalid(false)
            ,m_addrinfo(0)
            ,m_lasterrno(0)
{
    if ( m_family != AF_UNSPEC  )
    {
        m_isvalid = DetectLocalAddress(m_family, m_sockType);
    }
}

RNetAddress::RNetAddress(const sockaddr* addr, size_t addrlen)
			:m_family(-1)
			,m_sockType(0)
			,m_protocol(0)
            ,m_isvalid(false)
            ,m_addrinfo(0)
            ,m_lasterrno(0)
{
    if ( addr != NULL )
    {
        m_family = addr->sa_family;
        m_addrinfo = new (struct addrinfo);
        m_addrinfo->ai_flags = 0;
        m_addrinfo->ai_family = m_family;
        m_addrinfo->ai_socktype = 0;
        m_addrinfo->ai_protocol = 0;
        m_addrinfo->ai_addrlen = addrlen;
        m_addrinfo->ai_canonname = NULL;
        char* tmp = new char[addrlen];
        memcpy(tmp, addr, addrlen);
        m_addrinfo->ai_addr = (struct sockaddr*)tmp;
        m_addrinfo->ai_next = NULL;
        m_isvalid = true;
    }
}

RNetAddress::RNetAddress(int port, 
                         int family,
                         int sockType,
                         bool serverAddress) /*throw(RException)*/
			:m_family(family)
			,m_sockType(sockType)
			,m_protocol(0)
            ,m_isvalid(false)
            ,m_addrinfo(0)
            ,m_lasterrno(0)
{
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    if ( serverAddress == true )
        hints.ai_flags = AI_PASSIVE;
    hints.ai_family = family;
    if ( family != AF_UNSPEC && family != AF_INET && family != AF_INET6 )
		throw(RException("family is not in [0|AF_IENT|AF_INET6]", __FILE__, __LINE__));
    hints.ai_socktype = sockType;
    if ( sockType != 0 && sockType != SOCK_STREAM && sockType != SOCK_DGRAM && sockType != SOCK_RAW )
		throw(RException("socktype is not in [0|SOCK_STREAM|SOCK_DGRAM|SOCK_RAW]", __FILE__, __LINE__));

    int nret = -1;
    RString strPort;
    strPort.Format("%d", port);
    __startupWSock();
    if ( ( nret = getaddrinfo(NULL, strPort.c_str(), &hints, &result)) != 0 )
    {
        SetLastError(nret);
    }
    else
    {
        m_isvalid = true;
        m_addrinfo = result;
        m_hostName = m_addrinfo->ai_canonname;
    }
    __cleanupWSock();
}

RNetAddress::RNetAddress(const char* lpszip, 
                         int port, 
                         int family /*= AF_INET*/, 
                         int sockType /*= 0*/)
			:m_family(family)
			,m_sockType(sockType)
			,m_protocol(0)
            ,m_isvalid(false)
            ,m_addrinfo(0)
            ,m_lasterrno(0)
{
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_CANONNAME;	/* always return canonical name */
    hints.ai_family = family;
    if ( family != AF_UNSPEC && family != AF_INET && family != AF_INET6 )
		throw(RException("family is not in [0|AF_IENT|AF_INET6]", __FILE__, __LINE__));
    hints.ai_socktype = sockType;
    if ( sockType != 0 && sockType != SOCK_STREAM && sockType != SOCK_DGRAM && sockType != SOCK_RAW )
		throw(RException("socktype is not in [0|SOCK_STREAM|SOCK_DGRAM|SOCK_RAW]", __FILE__, __LINE__));
    int nret = 0;
    RString strPort;
    strPort.Format("%d", port);
    __startupWSock();
    if ( ( nret = getaddrinfo(lpszip, strPort.c_str(), &hints, &result)) != 0 )
    {
        SetLastError(nret);
    }
    else
    {
        m_isvalid = true;
        m_addrinfo = result;
        if ( m_addrinfo->ai_canonname != NULL )
            m_hostName = m_addrinfo->ai_canonname;
    }
    __cleanupWSock();
}

RNetAddress::RNetAddress(const RString& host,
                         int port,
                         int family,
                         int sockType) /*throw(RException)*/
			:m_family(family)
			,m_sockType(sockType)
			,m_protocol(0)
            ,m_isvalid(false)
            ,m_addrinfo(0)
            ,m_lasterrno(0)
{
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_CANONNAME;	/* always return canonical name */
    hints.ai_family = family;
    if ( family != AF_UNSPEC && family != AF_INET && family != AF_INET6 )
		throw(RException("family is not in [0|AF_IENT|AF_INET6]", __FILE__, __LINE__));
    hints.ai_socktype = sockType;
    if ( sockType != 0 && sockType != SOCK_STREAM && sockType != SOCK_DGRAM && sockType != SOCK_RAW )
		throw(RException("socktype is not in [0|SOCK_STREAM|SOCK_DGRAM|SOCK_RAW]", __FILE__, __LINE__));
    int nret = 0;
    RString strPort;
    strPort.Format("%d", port);
    __startupWSock();
    if ( ( nret = getaddrinfo(host.c_str(), strPort.c_str(), &hints, &result)) != 0 )
    {
        SetLastError(nret);
    }
    else
    {
        m_isvalid = true;
        m_addrinfo = result;
        if ( m_addrinfo->ai_canonname != NULL )
            m_hostName = m_addrinfo->ai_canonname;
    }
    __cleanupWSock();
}

RNetAddress::RNetAddress(const RString& host,
                         int port,
                         int flags,
                         int family,
                         int sockType,
                         int protocol)
			:m_family(family)
			,m_sockType(sockType)
			,m_protocol(protocol)
            ,m_isvalid(false)
            ,m_addrinfo(0)
            ,m_lasterrno(0)
{
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = flags;
    hints.ai_family = family;
    hints.ai_socktype = sockType;
    hints.ai_protocol = protocol;
        
    int nret = 0;
    RString strPort;
    strPort.Format("%d", port);
    __startupWSock();
    if ( ( nret = getaddrinfo(host.c_str(), strPort.c_str(), &hints, &result)) != 0 )
    {
        SetLastError(nret);
    }
    else
    {
        m_isvalid = true;
        m_addrinfo = result;
        m_hostName = m_addrinfo->ai_canonname;
    }
    __cleanupWSock();
}

RNetAddress::RNetAddress(const RString& host,
                         const RString& service,
                         int flags,
                         int family,
                         int sockType,
                         int protocol)
			:m_family(family)
			,m_sockType(sockType)
			,m_protocol(protocol)
            ,m_isvalid(false)
            ,m_addrinfo(0)
            ,m_lasterrno(0)
{
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = flags;
    hints.ai_family = family;
    hints.ai_socktype = sockType;
    hints.ai_protocol = protocol;
        
    int nret = 0;
    __startupWSock();
    if ( ( nret = getaddrinfo(host.c_str(), service.c_str(), &hints, &result)) != 0 )
    {
        SetLastError(nret);
    }
    else
    {
        m_isvalid = true;
        m_addrinfo = result;
        m_hostName = m_addrinfo->ai_canonname;
    }
    __cleanupWSock();
}

RNetAddress::RNetAddress(const RNetAddress& right)
            :m_addrinfo(NULL)
{
	m_family = right.m_family;
	m_sockType = right.m_sockType;
	m_protocol = right.m_protocol;
	m_isvalid  = right.m_isvalid;
	m_hostName = right.m_hostName;
	ResetLastErrno();

	int n = right.GetNumOfAddrinfo();
	struct addrinfo* left = NULL;
	struct addrinfo* lastaddrinfo = NULL;
	struct addrinfo* rightinfo = right.m_addrinfo;
	for (int i = 0; i < n; i++)		//¿½±´m_addrinfo
	{
		left = new (struct addrinfo);
		left->ai_flags = rightinfo->ai_flags;
		left->ai_family = rightinfo->ai_family;
		left->ai_socktype = rightinfo->ai_socktype;
		left->ai_protocol = rightinfo->ai_protocol;
		left->ai_addrlen = rightinfo->ai_addrlen;

        if ( rightinfo->ai_canonname != NULL )
        {
		    size_t len = strlen(rightinfo->ai_canonname) + 1;
		    left->ai_canonname = new char[len];
		    strncpy(left->ai_canonname, rightinfo->ai_canonname, len);
		    m_hostName = left->ai_canonname;
        }
        else
            left->ai_canonname = NULL;
		char* addr = new char[rightinfo->ai_addrlen];
		memcpy(addr, rightinfo->ai_addr, rightinfo->ai_addrlen);
		left->ai_addr = (struct sockaddr*)addr;
		left->ai_next = NULL;
		if ( m_addrinfo == NULL )
		{
			m_addrinfo = left;
		}
		else
		{
		    lastaddrinfo->ai_next = left;
		}
		lastaddrinfo = left;
		rightinfo = rightinfo->ai_next;
	}
}

RNetAddress& 
RNetAddress::operator=(const RNetAddress& right)
{
	if ( &right == this )
		return *this;
	m_family = right.m_family;
	m_sockType = right.m_sockType;
	m_protocol = right.m_protocol;
	m_isvalid  = right.m_isvalid;
	m_hostName = right.m_hostName;
	ResetLastErrno();

    freeaddrinfo(m_addrinfo);
    m_addrinfo = NULL;

	int n = right.GetNumOfAddrinfo();
	struct addrinfo* left = NULL;
	struct addrinfo* lastaddrinfo = NULL;
	struct addrinfo* rightinfo = right.m_addrinfo;
	for (int i = 0; i < n; i++)		//¿½±´m_addrinfo
	{
		left = new (struct addrinfo);
		left->ai_flags = rightinfo->ai_flags;
		left->ai_family = rightinfo->ai_family;
		left->ai_socktype = rightinfo->ai_socktype;
		left->ai_protocol = rightinfo->ai_protocol;
		left->ai_addrlen = rightinfo->ai_addrlen;

        if ( rightinfo->ai_canonname != NULL )
        {
    		size_t len = strlen(rightinfo->ai_canonname) + 1;
	    	left->ai_canonname = new char[len];
		    strncpy(left->ai_canonname, rightinfo->ai_canonname, len);
        }
        else
            left->ai_canonname = NULL;
		char* addr = new char[rightinfo->ai_addrlen];
		memcpy(addr, rightinfo->ai_addr, rightinfo->ai_addrlen);
		left->ai_addr = (struct sockaddr*)addr;
		left->ai_next = NULL;
		if ( m_addrinfo == NULL )
		{
			m_addrinfo = left;
		}
		else
		{
		    lastaddrinfo->ai_next = left;
		}
		lastaddrinfo = left;
		rightinfo = rightinfo->ai_next;
	}
	return *this;
}

RNetAddress::~RNetAddress()
{
    if (m_addrinfo)
    {
        freeaddrinfo(m_addrinfo);
        m_addrinfo = NULL;
    }
}

bool 
RNetAddress::IsValid() const
{
    return m_isvalid;
}

bool 
RNetAddress::DetectLocalAddress(int family, int sockType)
{
    if (__startupWSock() != 0)
        return false;
    /*
     *  NI_MAXHOST: max host length
     *  NI_MAXSERV: max service length
     */
    char hostname[NI_MAXHOST];
    int res = gethostname(hostname, sizeof(hostname));
    if (res != 0)
    {
        SetLastError(errno);
        __cleanupWSock();
        return false;
    }
    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(hints));
    hints.ai_flags = AI_CANONNAME;
    hints.ai_family = family;
    hints.ai_socktype = sockType;
    int nret = 0;
    if ( ( nret = getaddrinfo(hostname, NULL, &hints, &result)) != 0 )
    {
        SetLastError(nret);
        return false;
    }
    if ( m_addrinfo )
    {
        freeaddrinfo(m_addrinfo);
        m_addrinfo = NULL;
    }
    m_isvalid = true;
    m_hostName = hostname;
    m_addrinfo = result;
    __cleanupWSock();
    return true;
}

void 
RNetAddress::SetLastError(int errnum)
{
//#if defined(WIN32) || defined(_WIN32_WCE)
//    m_lasterrno = errnum;
//    m_lasterror = getSockError(m_lasterrno);
//#else
    m_isvalid = false;
    m_lasterrno = errnum;
    m_lasterror = gai_strerror(m_lasterrno);
//#endif
}

void 
RNetAddress::ResetLastErrno()
{
	m_lasterrno = 0;
	m_lasterror = "";
}

int 
RNetAddress::GetLastErrno() const
{
    return m_lasterrno;
}

const RString& 
RNetAddress::GetLastErrorDesc() const
{
    return m_lasterror;
}

int 
RNetAddress::GetNumOfAddr() const
{
    if ( !m_addrinfo )
    {
        return 0;
    }
    struct addrinfo* result = m_addrinfo;
    int i = 0;
    int socktype = 0;
    RString strtmp;
    for ( ; result != NULL; result = result->ai_next )
    {
        i++;
        socktype |= result->ai_socktype;
    }
    if ( socktype == (SOCK_STREAM | SOCK_DGRAM) )
        i /= 2;
    return i;
}

const RString&
RNetAddress::GetHostName() const
{
    return m_hostName;
}

int 
RNetAddress::GetNumOfAddrinfo() const
{
    int i = 0;
    struct addrinfo* result = m_addrinfo;
    for ( ; result != NULL; result = result->ai_next)
    {
        i++;
    }
    return i;
}

const SockAddr* 
RNetAddress::GetFirstSockAddr(int* family /*= 0*/, 
                              int* addrtype /*= 0*/, 
                              int* protocol /*= 0*/,
                              size_t* addrlen /*= 0*/) const
{
    if ( !m_addrinfo )
        return 0;
    if ( family )
        *family = m_addrinfo->ai_family;
    if ( addrtype )
        *addrtype = m_addrinfo->ai_socktype;
    if ( protocol )
        *protocol = m_addrinfo->ai_protocol;
    if ( addrlen )
        *addrlen = m_addrinfo->ai_addrlen;
    return m_addrinfo->ai_addr;
}

const SockAddr*  
RNetAddress::GetSockAddr(int nIndex /*= 0*/, 
                         int* family /*= 0*/, 
                         int* addrtype /*= 0*/, 
                         int* protocol /*= 0*/,
                         size_t* addrlen /*= 0*/) const
{
    if ( !m_addrinfo || nIndex >= GetNumOfAddrinfo() )
        return 0;

    struct addrinfo* result = m_addrinfo;
    for ( int i = 0; i < nIndex; i++ )
    {
        result = result->ai_next;
    }

    if ( family )
        *family = result->ai_family;
    if ( addrtype )
        *addrtype = result->ai_socktype;
    if ( addrlen )
        *addrlen = result->ai_addrlen;
    if ( protocol )
        *protocol = m_addrinfo->ai_protocol;

    return result->ai_addr;
}

const SockAddr* 
RNetAddress::GetLastSockAddr(int* family /*= 0*/, 
                             int* addrtype /*= 0*/, 
                             int* protocol /*= 0*/,
                             size_t* addrlen /*= 0*/) const
{
	int n = GetNumOfAddrinfo();
	return GetSockAddr(n - 1, family, addrtype, protocol, addrlen);
}

const RString 
RNetAddress::GetIP() const
{
    if ( m_addrinfo == NULL )
        return "";
    
    return GetIP(m_addrinfo->ai_addr);
}

const RString 
RNetAddress::GetIP(const SockAddr* sockaddress) const
{
    if ( !sockaddress )
        return "";
#if ( defined(WIN32) || defined(_win32) )
    /*
     *  windows don't support converting IPV6 to readable string,
     *  may user linux inet_ntop source code later
     */
    if ( sockaddress->sa_family  == AF_INET )
    {
        struct sockaddr_in* sockaddrin = (struct sockaddr_in*)sockaddress;
        return inet_ntoa(sockaddrin->sin_addr);
    }
    else if ( sockaddress->sa_family  == AF_INET6 )
        return "";
    else
        return "";
#else
    if ( sockaddress->sa_family  == AF_INET )
    {
        struct sockaddr_in* sockaddrin = (struct sockaddr_in*)sockaddress;
        char strip[INET_ADDRSTRLEN];
        if ( inet_ntop(sockaddrin->sin_family, &(sockaddrin->sin_addr), strip, sizeof(strip)) == NULL )
        {
            m_lasterrno = errno;
            m_lasterror = gai_strerror(m_lasterrno);
            return "";
        }
        return strip;
    }
    else if ( sockaddress->sa_family  == AF_INET6 )
    {
        struct sockaddr_in6* sockaddrin = (struct sockaddr_in6*)sockaddress;
        char strip[INET6_ADDRSTRLEN];
        if ( inet_ntop(sockaddrin->sin6_family, &(sockaddrin->sin6_addr), strip, sizeof(strip)) == NULL )
        {
            m_lasterrno = errno;
            m_lasterror = gai_strerror(m_lasterrno);
            return "";
        }
        return strip;
    }
    else
        return "";
#endif
    return "";
}

int 
RNetAddress::GetPort() const
{
    if ( m_addrinfo == NULL )
        return 0;
    return GetPort(m_addrinfo->ai_addr);
}

int 
RNetAddress::GetPort(const SockAddr* sockaddress) const
{
    if ( !sockaddress )
        return 0;
#if ( defined(WIN32) || defined(_win32) )
    /*
     *  windows don't support converting IPV6 to readable string,
     *  may user linux inet_ntop source code later
     */
    if ( sockaddress->sa_family  == AF_INET )
    {
        struct sockaddr_in* sockaddrin = (struct sockaddr_in*)sockaddress;
        return ntohs(sockaddrin->sin_port);
    }
    else if ( sockaddress->sa_family  == AF_INET6 )
        return 0;
    else
        return 0;
#else
    if ( sockaddress->sa_family  == AF_INET )
    {
        struct sockaddr_in* sockaddrin = (struct sockaddr_in*)sockaddress;
        return ntohs(sockaddrin->sin_port);
    }
    else if ( sockaddress->sa_family  == AF_INET6 )
    {
        struct sockaddr_in6* sockaddrin = (struct sockaddr_in6*)sockaddress;
        return ntohs(sockaddrin->sin6_port);
    }
    else
        return 0;
#endif
    return 0;
}

void 
RNetAddress::PrintNetAddress() const
{
    if ( !m_addrinfo )
    {
        printf("m_addrinfo is null\n");
        return;
    }
    struct sockaddr_in *sockaddrin;
    struct addrinfo* result = m_addrinfo;
    int i = 1;
    for ( ; result != NULL; result = result->ai_next, i++ )
    {
        printf("%d: struct addrinfo result's member:\n", i);
        printf("\tresult->ai_flags: %d, AI_PASSIVE is %d, AI_CANONNAME is %d, AI_NUMERICHOST is %d\n", result->ai_flags,AI_PASSIVE,AI_CANONNAME, AI_NUMERICHOST );
        printf("\tresult->ai_family: %d, AF_INET is %d, AF_INET6 is %d\n", result->ai_family, AF_INET, AF_INET6);
        printf("\tresult->ai_socktype: %d, SOCK_STREAM is %d, SOCK_DGRAM is %d\n", result->ai_socktype, SOCK_STREAM, SOCK_DGRAM);
        printf("\tresult->ai_protocol: %d\n", result->ai_protocol);
        printf("\tresult->ai_addrlen: %d\n", result->ai_addrlen);
        printf("\tresult->ai_canonname: %s\n", result->ai_canonname);
        printf("\t--------------------------------\n");
        char* strip = (char*)malloc(result->ai_addrlen);
        printf("\tstruct addrinfo result->ai_addr's menber:\n");
        sockaddrin = (struct sockaddr_in*)(result->ai_addr);
        //printf("\tresult->ai_addr->sin_len:%d\n", sockaddrin->sin_len);
        printf("\tresult->ai_addr->sin_family:%s\n",sockaddrin->sin_family==AF_INET?"AF_INET":"AF_INET6");
        printf("\tresult->ai_addr->sin_port:%d\n",ntohs(sockaddrin->sin_port));
#if ( defined(WIN32) || defined(_win32) )
        printf("\tresult->ai_addr->sin_addr:%s\n\n", inet_ntoa(sockaddrin->sin_addr));
#else
        printf("\tresult->ai_addr->sin_addr:%s\n",inet_ntop(result->ai_family, &(sockaddrin->sin_addr), strip, result->ai_addrlen));
#endif
        free(strip);
    }
}
