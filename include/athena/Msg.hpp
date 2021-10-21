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

#ifndef RMSGQUEUE_HPP_HEADER_INCLUDED
#define RMSGQUEUE_HPP_HEADER_INCLUDED

#include <sys/types.h>
#define	MAX_MSG_SIZE	8192
typedef struct _MessageQueueBuf
{
	long mtype;
	char mtext[1];
}MqBuf;

class CMsg
{
public:
    CMsg(int key, size_t maxMsgSize = MAX_MSG_SIZE);

    virtual ~CMsg();

    /*
     *  return
     *      -1 create or get semaphore error
     *      0  get existd semaphore success
     *      1  created semaphore success
     */
    int GetMsg();

    /*
     *  params:
     *      buf ������Ϣ�Ļ���
     *      msgsize ������Ϣ�ĳ���
     *      priority ��Ϣ���ͣ���������ʶ�ĸ��ͻ��ˣ��������0
     *  return:
     *      success ������Ϣ���ֽ����������Ϣ��������(ϵͳ)��������
     *      failure -1
     */
    int Send(const char* buf, size_t bufLen, unsigned priority = 1);

    /*
     *  return:
     *      -2  No message was available in the queue
     */
    int TrySend(const char* buf, size_t bufLen, unsigned priority = 1);

    /*
     *  params:
     *      buf ������Ϣ�Ļ���
     *      masmsgsize ��Ϣ����󳤶�
     *      priority ��Ϣ���ͣ���������ʶ�ĸ��ͻ���;���Ϊ0���򲻹���Ϣ���ͣ�ֻȡ��һ����Ϣ
     *  return:
     *      success ȡ�õ���Ϣ���ȣ�û����Ϣ������.
     *      failure -1  ������Ϣ����,���maxmsgsizeС����Ϣ�ĳ��ȣ�Ҳ�����ش���;
     */
    int Recv(MqBuf** buf, unsigned priority = 0);

    /*
     *  return:
     *      -2  The message cana't be sent due to the msg_qbytes limit for the queue
     */
    int TryRecv(MqBuf** buf, unsigned priority = 0);

    /*
     *  ���ص�����Ϣ���еĳ���
     */
    int Size() const;

    bool Remove();

private:
    CMsg(const CMsg&);        //��ֹ��Ϣ���п���

    const CMsg& operator=(const CMsg&);   //��ֹ��ֵ

private:
	int m_nKey;

	int m_nMsqID;

    MqBuf* m_msgBuf;

    size_t   m_maxMsgSize;
};

#endif /* RMSGQUEUE_HPP_HEADER_INCLUDED */
