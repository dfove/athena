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

#ifndef RNET_ADDRESS_HPP_HEADER_INCLUDED
#define RNET_ADDRESS_HPP_HEADER_INCLUDED

/*
 *  网络IP地址：端口
 *      支持IPV4\IPV6
 *  todo:IPV6
 *
 *========================================================================
 *  通用套接口地址结构
 *========================================================================
 *  struct sockaddr
 *  {
 *      uint8_t         sa_len;
 *      sa_family_t     sa_family;      //address family:AF_XXX value
 *      char            sa_data[14];    //protocol-specific address
 *  };
 *
 *========================================================================
 *  getaddrinfo函数的参数
 *========================================================================
 *  struct addrinfo
 *  {
 *      int             ai_flags;       //AI_PASSIVE, AI_CANONNAME
 *      int             ai_family;      //AF_XXX
 *      int             ai_socktype;    //SOCK_XXX
 *      int             ai_protocol;    //0 or IPPROTO_XXX for IPV4 and IPV6
 *      size_t          ai_addrlen;     //length of ai_addr
 *      char*           ai_canonname;   //ptr to canonical name for host
 *      struct sockaddr*    ai_addr;    //ptr to socket address structure
 *      struct addrinfo*    ai_next;    //ptr to next structure in linked list
 *  };
 *
 *========================================================================
 *  IPV4 : 
 *========================================================================
 *  struct in_addr
 *  {
 *      in_addr_t       s_addr;         //32bits, IPV4 address
 *  };
 *
 *  struct sockaddr_in
 *  {
 *      uint8_t         sin_len;        //8 bits, length of struct(==16 bytes)
 *      sa_family_t     sin_family;     //8 bits, AF_INET
 *      in_port_t       sin_port;       //16 bits, TCP or UDP port
 *      struct in_addr  sin_addr;       //32 bits, IPV4 address
 *      char            sin_zeor[8];    //unused
 *  };
 *
 *========================================================================
 *  IPV6 : 
 *========================================================================
 *  struct in6_addr
 *  {
 *      uint8_t         s6_addr[16];    //128 bits, IPV6 address
 *  };
 *
 *  struct sockaddr_in6
 *  {
 *      uint8_t         sin6_len;       //8 bits, length of struce(==24 bytes)
 *      sa_family_t     sin6_family;    //8 bits, AF_INET6
 *      in_port_t       sin6_port;      //16 bits, TCP or UDP port
 *      uint32_t        sin6_flowinfo;  //32 bits, priority & flow label
 *      struct in6_addr sin6_addr;      //128 bits, IPV6 address
 *  };
 */

#include <Unidef.hpp>
#include <RNetUnidef.hpp>
#include <RString.hpp>
#include <RException.hpp>

//#ifdef ENABLE_IPV6
//typedef struct sockaddr_in6 SockAddr_in;
//#else
//typedef struct sockaddr_in  SockAddr_in;
//#endif
typedef struct sockaddr SockAddr;

namespace athena
{

/*
 *  struct sockaddr的封装与转换(指定了特定的family和socktype)
 *  对于域名和端口的组合，将有可能得到多个struct addrinfo,即m_addrinfo是个链表
 *  对于IP和端口，只得到一个struct addrinfo,即m_addrinfo是个只有一个元素的链表
 */
class ATHENA_EX RNetAddress
{
public:
	/*
     *  获取本地所有sockaddr
     *  params:
     *    family        ip地址族
     *      AF_UNSPEC
     *  	AF_INET     IPV4地址
     *  	AF_INET6    IPV6地址
     *  	AF_LOCAL
     *  	AF_ROUTE
     *  	AF_KEY
     *    socktype      sockaddr类型,是TCP的还是UDP的(addrinfo所包含的类型)
     *  	SOCK_STREAM TCP
     *  	SOCK_DGRAM  UDP
     *  	SOCK_RAW
     *  1、如果family为默认值0，则不做任何操作
     *  2、如果family为AF_INET或AF_INET6，则获取本地IP(v4或v6)地址
     *  3、sockType如果为0，则取得2中类型的addrinfo(tcp或udp)，以下相同
     */
    explicit RNetAddress(int family = AF_UNSPEC, int sockType = 0);

    /*
     *  将sockaddr转换为更人性化的RNetAddress
     *  params:
     *      addr    struct sockaddr_in*     ipv4
     *              struct sockaddr_in6*    ipv6 
     *      addrlen addr的长度,与addr实际长度不匹配可能会导致出错
     */
    RNetAddress(const sockaddr* addr, size_t addrlen);

    /*
     *  The address for server application(accept) or client application;
     *  params:
     *      port: Should be used port
     *      family、socktype: as 'int socket(int domain, int type, int protocol)' params
     *      serverAddress: true, for the server application, the network address should no be set, as INADDR_ANY
     *                     false, for the client application, and the network address shoule be set to the loopback address
     */
    RNetAddress(int port, 
                int family /*= AF_UNSPEC*/,
                int sockType /*= 0*/,
                bool serverAddress = true) /*throw(RException)*/;

    RNetAddress(const char* lpszip, 
                int port, 
                int family = AF_UNSPEC, 
                int sockType = 0);

    /*
     *  The address for connect(server application)
     *      official name of the host
     */
    RNetAddress(const RString& host,
                int port,
                int family = AF_UNSPEC, 
                int socktype = 0) /*throw(RException)*/;

    /*
     *  the wrapper of the getaddrinfo
     *  params:
     *      host:ip or domain name
     *      port:number port
     *      service:service name
     *      flags:as ai_flags, AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST...
     *      family:as ai_family, AF_INET, AF_INET6
     *      socktype: as ai_socktype, SOCK_STREAM, SOCK_DGRAM
     *      protocol: as ai_protocol
     */
    RNetAddress(const RString& host,
                int port,
                int flags,
                int family,
                int socktype,
                int protocol);

    RNetAddress(const RString& host,
                const RString& service,
                int flags,
                int family,
                int socktype,
                int protocol);

    RNetAddress(const RNetAddress & right);

    RNetAddress& operator=(const RNetAddress& right);

    virtual ~RNetAddress();

    bool IsValid() const;

    bool DetectLocalAddress(int family, int sockType);

	void ResetLastErrno();

    int GetLastErrno() const;

    const RString& GetLastErrorDesc() const;

    int GetNumOfAddr() const;

    const RString& GetHostName() const;

    int GetNumOfAddrinfo() const;

    const SockAddr* GetFirstSockAddr(int* family = 0, 
                                     int* addrtype = 0, 
                                     int* protocol = 0,
                                     size_t* addrlen = 0) const;

    const SockAddr* GetSockAddr(int nIndex, 
                                int* family = 0, 
                                int* addrtype = 0, 
                                int* protocol = 0,
                                size_t* addrlen = 0) const;

    const SockAddr* GetLastSockAddr(int* family = 0, 
                                    int* addrtype = 0, 
                                    int* protocol = 0,
                                    size_t* addrlen = 0) const;

    const RString GetIP() const;

    const RString GetIP(const SockAddr* sockaddress) const;

    int GetPort() const;

    int GetPort(const SockAddr* sockaddress) const;

	int Family() const{return m_family;}

	int SockType() const{return m_sockType;}

	int Protocol() const{return m_protocol;}

    void PrintNetAddress() const;

private:
    void SetLastError(int errnum);

private:
	int					m_family;

	int 				m_sockType;

	int 				m_protocol;

    bool                m_isvalid;

    RString             m_hostName;

    struct addrinfo*    m_addrinfo;

    mutable int         m_lasterrno;

    mutable RString     m_lasterror;
};

}

#endif /* RNET_ADDRESS_HPP_HEADER_INCLUDED */
