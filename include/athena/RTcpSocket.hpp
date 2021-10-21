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

#ifndef RTCPSOCKET_HPP_HEADER_INCLUDED
#define RTCPSOCKET_HPP_HEADER_INCLUDED

#include <RSocket.hpp>
#include <RStream.hpp>

namespace athena
{

class ATHENA_EX RTcpSocket : public RSocket
{
public:
    typedef enum _TcpErrno {
        E_Success	=		0,          //�ɹ�
        E_Argv	    =       1,          //����ȷ�Ĳ���
        E_Create	=		2,          //����socketʧ��
        E_Sockfd    =       3,          //��Ч��socket fd
        E_Bind		=		4,          //bind�˿�ʧ��
        E_Listen	=		5,          //listenʧ��
        E_Connect	=		6,          //connect���Ӵ���
        E_Accept    =       7,          //acceptʧ��
        E_Send		=		8,          //send�������ݹ��̴���
        E_Recv		=		9,          //recv�������ݹ��̴���
        E_Notconn   =       10,         //û������
        E_Timeout	=		11,         //��ʱ(ָ��ʱ�������ɶ�д)
        E_WOULDBLOCK =      12,         //���ڷ�����
        E_Breaken	=		13,         //�����ѶϿ�
        E_Host		=		14,         //����IP��ַ����
        E_Port		=		15,         //�˿ڴ���
        E_Set		=		16,         //����socketʧ��
        E_INTERRUPT =       17          //�ж�
    }TcpErrno;

    typedef enum _AddressFamily
    {
        IPV4 = 0,
        IPV6
    }MyIpType;

public:
    /*
     *	����:����tcp socket
     *
     *	����family,socktype,protocol�μ�ϵͳ����socket�Ĳ���˵��
     *	family:
     *		AF_INET
     *		AF_INET6 
     *	bAutoClose:
     *		���ٴ���ʱ���Զ��ر���Ӧ��socket fd
     */
    RTcpSocket(MyIpType addressFamily = IPV4, int protocol = 0, bool bAutoClose = true);

    /*
     *  params:
     *      sockfd  accept���Ӻ��sockfd
     *      bBlockType Ĭ��Ϊ������ʽtrue;��������ʽfalse
     *      bAutoClose  �������ٵ�ʱ���Զ��ر�sockfd
     */
    RTcpSocket(SOCKET sockfd, bool bBlock = true, bool bAutoClose = true);

    /*
     *  �����Զ�bind��socket
     *
     *  params:
     *      localAddr   �Զ��󶨵ı��ص�ַ
     *      bAutoClose  �Ƿ������ٵ�ʱ���Զ��ر�socket
     */
    RTcpSocket(const RNetAddress & localAddr, bool bAutoClose = true);

    virtual ~RTcpSocket();

    /*
     *  �����׽��֣����֮ǰ�׽��ֻ���Ч����Ϊ�������ظ��׽���id��������Ϊ�µ��׽ӿ�fd
     *  params:
     *      fd  �µ����������������׽���id
     *      bConnected  �Ƿ��Ѿ����ӳɹ�������Ϊ����
     *      addressFamily   IPV4 or IPV6
     *      protocol    Э���ֶ�
     *      bBlock      �Ƿ�����
     *      autoClose   �Ƿ��Զ��ر�
     */
    int AttachSocketFd(SOCKET fd, 
                       bool bBlock = true,
                       bool bConnected = true,
                       MyIpType addressFamily = IPV4, 
                       int protocol = 0,
                       bool autoClose = true);

private:
    RTcpSocket(const RTcpSocket& right);

    RTcpSocket& operator=(const RTcpSocket& right);

public:
    /*
     *  ���socket��û�д��������Զ�����socket�������ӷ�������ַ�Ͷ˿�
     *  ���ڷ�����socket�����趨��ʱʱ�䣬����ʱ��󼴷���false
     *  ��������socket,Ĭ�ϵĳ�ʱʱ����Ϊ75��(linux .etc)
     */
	bool Connect(const RString& host, int nPort, unsigned waitMilliseconds = 10*1000);

    bool Connect(const RNetAddress& remoteAddr, unsigned waitMilliseconds = 10*1000);

    virtual bool Close();

    bool IsConnected() const;

    /*
     *  �Ѱ󶨻�δ�󶨵�ַ�Ͷ˿�
     *      ���δ�����ָ��IP�Ͷ˿�
     *      ����Ѱ󶨣���ʹ��֮ǰ�Ѱ󶨵ĵ�ַ�Ͷ˿ڣ�����ָ���ĵ�ַ�Ͷ˿�
     *  params:
     *      maxconn ���������
     *      port    �����˿�.   
     *              Ĭ��ֵ=-1   socket�Ѿ�Bind
     *              �˿�ֵ>0    socketû��Bind,������֮ǰҪ�Ȱ�
     *      lpziIP  ����IP��Ĭ��ΪINADDR_ANY
     *  return(_TcpErrno):
     *      E_Port
     *      E_Host
     *      E_Bind  ���֮ǰû�е���Bind��������Listen�е���ʧ��ʱ����
     *      E_Listen
     *      E_Success
     */
    int Listen(int maxconn, 
               int port = -1, 
               const char* lpszIP = NULL);

    /*
     *  return:
     *      socket fd
     */
    SOCKET Accept(const unsigned uMilliseconds = 10);                        //������������

    SOCKET Accept(RString& sIP, unsigned& uPort, const unsigned uMilliseconds = 10);    //�����������󣬲��õ��Է���IP��port


    /*
     *  ���ڷ���������socket, ���趨��ʱʱ�䣬����Ԥ��ʱ�䷵��false
     *  ������������socket, һֱ����ֱ���ͻ��������ӳɹ�
     *  params:
     *      connfd          ������socket
     *      waitMilliseconds    ������accept�ȴ�ʱ�䣬����
     *      clientIP        Զ�����ӵ�ip
     *      clientPort      Զ�����ӵĶ˿�
     *  return(_TcpErrno):
     *      E_Sockfd
     *      E_Timeout
     *      E_Accept
     *      E_Success
     */
    int Accept(RTcpSocket& connfd, unsigned waitMilliseconds = 100);

    int Accept(RTcpSocket& connfd, RString& clientIP, int& clientPort, unsigned waitMilliseconds = 100);

    /*
     *  Send\Recv, ����\�������nSize�ֽڵ�����
     *  return:
     *      >= 0 �ɹ�����\���յ��ֽ���
     *      SOCKET_ERROR  ʧ��
     */
    int SendOnce(const char* pData, int nbytes);

    int SendOnce(const RString& buf);

    int SendOnce(const RStream& stream);

    int RecvOnce(char* pData, int nbytes);

    int RecvOnce(RString& buf, int nbytes);

    int RecvOnce(RStream& stream, int nbytes);

    /*
     *  Sendn\Recvn�����͡����յ���nSize�ֽڵ�����
     *  params:
     *      waitMilliseconds (Recvnͬ)
     *                   ��λ������
     *                   > 0    �ȴ�������������/����������ͬ���ж���ĳʱ������Ƿ�ɶ�(recv)д(read)��
     *                          ��ʱʱ��waitMilliseconds�󷵻�E_Timeout
     *                   0      ���ȴ������ɶ�д������ʧ��E_Timeout
     *                   -1     ��������һֱ�ȴ���ֱ���ɶ�(recv)д(read)��
     *                          ���ڷ����������û�����ݿɶ�(recv)д(read)������E_WOULDBLOCK
     *  return(_TcpErrno):
     *      E_Sockfd
     *      E_Timeout
     *      E_WOULDBLOCK   //���ڷ�����
     *      E_Send
     *      E_Success
     */
	int Sendn(const char* pData, int nbytes, int waitMilliseconds = -1);

    int Sendn(const RString& buf, int waitMilliseconds = -1);

    int Sendn(const RStream& stream, int waitMilliseconds = -1);

    /*
     *  params:
     *      waitMilliseconds    ͬSendn
     *  pData���Զ�����'\0'��β
     *  return(_TcpErrno):
     *      E_Sockfd
     *      E_Timeout
     *      E_WOULDBLOCK    //���ڷ�����
     *      E_Recv
     *      E_Breaken
     *      E_Success
     */
	int Recvn(char* pData, int nbytes, int waitMilliseconds = -1);

    int Recvn(RString& buf, int nbytes, int waitMilliseconds = -1);

    int Recvn(RStream& stream, int nbytes, int waitMilliseconds = -1);

private:
    /*
     *  ������socket
     */
    bool    m_isListening;

    /*
     *  �������Ӷ˿�(�ͻ��˻�����accept�ɹ���socket)
     */
    bool    m_isConnected;

};

};  /*namespace*/

#endif /*RTCPSOCKET_HPP_HEADER_INCLUDED*/
