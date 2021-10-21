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
     *      buf 发送消息的缓存
     *      msgsize 发送消息的长度
     *      priority 消息类型，可用来标识哪个客户端，必须大于0
     *  return:
     *      success 发送消息的字节数，如果消息队列已满(系统)，则阻塞
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
     *      buf 接收消息的缓存
     *      masmsgsize 消息的最大长度
     *      priority 消息类型，可用来标识哪个客户端;如果为0，则不管消息类型，只取第一个消息
     *  return:
     *      success 取得的消息长度，没有消息则阻塞.
     *      failure -1  接收消息错误,如果maxmsgsize小于消息的长度，也将返回错误;
     */
    int Recv(MqBuf** buf, unsigned priority = 0);

    /*
     *  return:
     *      -2  The message cana't be sent due to the msg_qbytes limit for the queue
     */
    int TryRecv(MqBuf** buf, unsigned priority = 0);

    /*
     *  返回当天消息队列的长度
     */
    int Size() const;

    bool Remove();

private:
    CMsg(const CMsg&);        //禁止消息队列拷贝

    const CMsg& operator=(const CMsg&);   //禁止赋值

private:
	int m_nKey;

	int m_nMsqID;

    MqBuf* m_msgBuf;

    size_t   m_maxMsgSize;
};

#endif /* RMSGQUEUE_HPP_HEADER_INCLUDED */
