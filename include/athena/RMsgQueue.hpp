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

#include <Unidef.hpp>
#include <RString.hpp>

namespace athena
{
class RStream;

class RMsgQueue
{
struct RMsg_t;

public:
    RMsgQueue();
    /*
     *  消息队列，暂不支持windows
     *  params: 
     *      key  消息队列标识
     *      name 消息队列名称
     */
    RMsgQueue(unsigned key);

    RMsgQueue(const char* name);

    virtual ~RMsgQueue();

    void Init(unsigned key);

    void Init(const RString& name);

    /*
     *  params:
     *      buf 发送消息的缓存
     *      msgsize 发送消息的长度
     *      priority 消息类型，可用来标识哪个客户端，必须大于0
     *  return:
     *      发送消息是否成功，如果消息队列已满(系统)，则阻塞
     */
    bool Send(const char* buf, size_t msgsize, unsigned priority = 1);

    bool Send(const RStream& stream, unsigned priority = 1);

    bool TrySend(const char* buf, size_t msgsize, unsigned priority = 1);

    bool TrySend(const RStream& stream, unsigned priority = 1);
    /*
     *  params:
     *      buf 接收消息的缓存
     *      masmsgsize 消息的最大长度,对于POSIX无效，只针对SYSTEM V有效
     *      priority 消息类型，可用来标识哪个客户端;如果为0，则不管消息类型，只取第一个消息
     *               对于POSIX消息队列无效，始终返回最高优先级（类型）消息
     *  return:
     *      -1  接收消息错误,如果maxmsgsize小于消息的长度，也将返回错误;
     *      取得的消息长度，没有消息则阻塞.
     */
    int Recv(char* buf, size_t maxmsgsize, unsigned priority = 0);

    int Recv(RStream& stream, unsigned priority = 0);

    int TryRecv(char* buf, size_t maxmsgsize, unsigned priority = 0);

    int TryRecv(RStream& stream, unsigned priority = 0);

    const RString& Name() const;

    /*
     *  返回当天消息队列的长度
     */
    int Size() const;

    bool Remove();
private:
    RMsgQueue(const RMsgQueue&);        //禁止消息队列拷贝

    const RMsgQueue& operator=(const RMsgQueue&);   //禁止赋值

private:
    RString         m_name;

    struct RMsg_t*  m_msg;
};

}       /*namespace*/

#endif /* RMSGQUEUE_HPP_HEADER_INCLUDED */
