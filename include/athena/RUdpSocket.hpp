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

#ifndef RUDPSOCKET_HPP_HEADER_INCLUDED
#define RUDPSOCKET_HPP_HEADER_INCLUDED

#include <RSocket.hpp>
#include <RStream.hpp>
#include <RNetAddress.hpp>

namespace athena
{

class ATHENA_EX RUdpSocket : public RSocket
{
public:
	/*
	 *	params:
	 *		addrFamily 	AF_INET		IPv4
	 *					AF_INET6	IPv6
	 *	����ͻ���û���ṩlocalAddress,�򲻰�ip��port,ֱ��Connect����send��ʱ�����������ip:port
	 *	����󶨾����ip:port
	 */
	RUdpSocket(int addrFamily = AF_INET, bool bAutoClose = true);

	RUdpSocket(const RNetAddress& localAddress, bool bAutoClose = true);

	virtual ~RUdpSocket(); 

    /*
     *���Ὠ��һ��"��ʵ"�����ӣ�ֻ�����ں˱���Է���IP��ַ�Ͷ˿ں�;
     *������Է������κ����ݱ��������Щ������˿ڲ��ɴ��ָ���˿�û�з�����������������Է��������ݱ�ǰ���ܷ���
     */
	bool Connect(const RString& host, int nPort);

	bool Connect(const RNetAddress& remoteAddr);

    bool IsConnected() const;

    /*
     *  ����������socket
     */
	int Send(const char* pData, int nSize);

    /*
     *  praams:
     *      buf\stream �������ַ���
     *      nbytes  ���ݱ�����
     *      ip��port    ����Զ��IP�Ͷ˿�
     *      waitMilliseconds    �ȴ�socket�ɶ��ĳ�ʱ�������λ������
     *          >   �ȴ�ʱ�䣬��ʱsocket������д���򷵻�ʧ��
     *          =0  ���ȴ���socket����д�ͷ���ʧ��
     *          -1  һֱ�ȵ�socket��д
     *  return:
     *      SOCKET_ERROR    ����
     *      >=0             �����ֽ���
     */
	int SendTo(const char* buf, int nbytes, 
	           const char* ip, int port, 
	           int waitMilliseconds = -1);

	int SendTo(const char* buf, int nbytes, 
	           const RNetAddress& remoteAddr,
	           int waitMilliseconds = -1);

    int SendTo(const RString& buf, 
               const RNetAddress& remoteAddr,
               int waitMilliseconds = -1);

    int SendTo(const RStream& stream, 
               const RNetAddress& remoteAddr,
               int waitMilliseconds = -1);

    /*
     *  params:
     *      buf\stream  ���ջ���
     *      nbytes      buf�ĳ���,�ɽ��յ���󳤶ȣ���λ���ֽ�
     *      ip\port     Զ��IP��ַ�Ͷ˿�
     *      waitMilliseconds    �ȴ�socket�ɶ��ĳ�ʱ���
     *          >0  �ȴ�ʱ�䣬��ʱ��û�����ݣ��򷵻�ʧ��
     *          =0  ���ȴ���socket���ɶ�������ʧ��
     *          -1  һֱ�ȵ�socket�ɶ�
     *  return:
     *      SOCKET_ERROR    ����
     *      >0              ���յ��ֽ���
     *      =0              ������socket���ر�
     */
	int RecvFrom(char* buf, int nbytes,
	             char* ip = NULL, int* port = NULL,
	             int waitMilliseconds = -1);

	int RecvFrom(char* buf, int nbytes, 
	             RNetAddress& remoteAddr, 
	             int waitMilliseconds = -1);

    int RecvFrom(RString& buf, int nbytes,
                 RNetAddress& remoteAddr,
                 int waitMilliseconds = -1);

    int RecvFrom(RStream& stream, int nbytes,
                 RNetAddress& remoteAddr,
                 int waitMilliseconds = -1);

private:
    bool            m_isConnected;

};

}   /*namespace*/

#endif /*RUDPSOCKET_HPP_HEADER_INCLUDED*/
