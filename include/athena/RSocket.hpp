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
     *	����family,socktype,protocol�μ�ϵͳ����socket�Ĳ���˵��
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
     *		һ������Ϊ0�������õ�ԭʼ�׽ӿ���
     *	����:����ָ��Э���socket
     *		���socket�Ѵ������򷵻��Ѵ��ڵ�socket fd
     *		����´����ɹ����򷵻ش�����socket fd
     *		��������SOCKET_ERROR(-1)
     */
    SOCKET Create(int family = AF_INET, int socktype = SOCK_STREAM,int protocol = 0);

//    /*
//     *  �����׽��֣����֮ǰ�׽��ֻ���Ч���ر��׽��֣�������Ϊ�µ��׽ӿ�fd
//     */
//    bool SetSocketFd(SOCKET fd, 
//                     int socktype, 
//                     int protocol = 0,
//                     bool blocktype = true,
//                     bool autoClose = true);

	SOCKET GetSocketFd() const;

    bool IsValid() const;

    /*
     *  ����TCP/UDP���ɰ�IP�Ͷ˿ڣ�һ����Ҫ�ǰ󶨶˿�
     *  params:
     *      ip      IP��ַ��TCP/UDP���ΪNULL�����ͨ���ַ
     *                      Raw socket����Ϊ��
     *      port    �˿ڣ�TCP/UDP�����<=0�������ں�ָ���˿�
     *                      Raw socket��Ч
     */
    bool Bind(int port, const char* ip = NULL);

    bool Bind(const SockAddr* addr, socklen_t addrlen);

    bool Bind(const RNetAddress& localAddr);

    virtual bool Close();

    bool IsClosed() const;

    void Shutdown(SHUTDOWN_TYPE type);

    SHUTDOWN_TYPE GetShutdownType() const;

	/*
	 *	���׽ӿڹ����ı���Э���ַ
	 *		1��������bind����connect�ɹ��󣬹����ı���IP��ַ�ͱ��ض˿ں�
	 *		2���Ա��ض˿�0����bind�󣬹����ı��ض˿ں�
	 *		3����Ϊ��������������ͨ��IP��ַ����ͻ��������Ӻ�(accept�ɹ�)������������ӵı���IP��ַ
	 *
     *  wrapper of getsockname
     *      windows:�ڰ󶨱��ص�ַ�Ͷ˿�֮����Ч��û��bind�򷵻�ʧ��
     *      linux  :û��bindҲ��Ч�����صĵ�ַΪͨ���ַ���˿�Ϊ0
     *  ȡ�����׽ӿڹ����ı���Э���ַ
     *  return:
     *      �׽ӿڵĵ�ַ��,����ʱ����-1
     */
    int GetSockName(RNetAddress& sockname);

	/*
	 *	���׽ӿڹ�����Զ��Э���ַ,����Զ�̽�������֮�����
	 *
     *  wrapper of getpeername
     *      û��connect���򷵻�ʧ��
     *  ȡ�����׽ӿڹ�����Զ��Э���ַ
     *  return:
     *      �׽ӿڵĵ�ַ��,����ʱ����-1
     */
    int GetPeerName(RNetAddress& peername);

    /*
     *  ��Ԥ����ʱ�����Ƿ������ݿɶ�д������쳣typedef enum 
     *  {
     *      FD_Readable = 1,
     *      FD_Writeable = 2,
     *      FD_Errable = 4
     *  }
     *  MYFDSET;
     *  params:
     *      myfdset     enum�ṹMYFDSET��һ����߶���Ļ�ֵ,
     *                  ��Ҫ�ж��Ƿ�ɶ�д��Ϊ: FD_Readable | FD_Writeable
     *      millisecond ����
     *                  -1��<0  ���õȴ���ֱ���ɶ�д������쳣
     *                  0   ���ȴ����ж��Ƿ�ɶ�д���쳣��ֱ�ӷ��ؽ��
     *                  >0  �ȴ���ʱ�䣬�ж���Ԥ��ʱ�����Ƿ������ݿɶ�д���쳣
     *      [out]readable   ����ɶ�����ֵtrue
     *      [out]writeable  �����д����ֵtrue
     *      [out]errable    ����쳣����ֵtrue
     *  return:
     *      �趨��myfdset����Ҫ�󣬷���true�����򷵻�false
     *      ֵ-��������ķ���Ҳ������myfdset�ķ�Χ
     *      ��:myfdset == FD_Readable | FD_Writeable,��ʹ�����쳣,*errableҲ�����ı�
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
     *  ����socket�Ƿ�����
     *  params:
     *      bBlock  true    ����Ϊ����
     *              false   ����Ϊ������
     */
	bool SetBlockType(bool bBlock); 

    /*
     *  ����socket�Ƿ������
     *  return:
     *      true    ����socket
     *      false   ������socket
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
     *      �׽ӿڵĵ�ַ��
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
     *  �Ƿ�����socket
     */
    bool            m_blocktype;

	/*
	 *	�Ƿ��ڶ������ٵ�ʱ��ر�socket
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
