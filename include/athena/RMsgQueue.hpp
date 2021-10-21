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
     *  ��Ϣ���У��ݲ�֧��windows
     *  params: 
     *      key  ��Ϣ���б�ʶ
     *      name ��Ϣ��������
     */
    RMsgQueue(unsigned key);

    RMsgQueue(const char* name);

    virtual ~RMsgQueue();

    void Init(unsigned key);

    void Init(const RString& name);

    /*
     *  params:
     *      buf ������Ϣ�Ļ���
     *      msgsize ������Ϣ�ĳ���
     *      priority ��Ϣ���ͣ���������ʶ�ĸ��ͻ��ˣ��������0
     *  return:
     *      ������Ϣ�Ƿ�ɹ��������Ϣ��������(ϵͳ)��������
     */
    bool Send(const char* buf, size_t msgsize, unsigned priority = 1);

    bool Send(const RStream& stream, unsigned priority = 1);

    bool TrySend(const char* buf, size_t msgsize, unsigned priority = 1);

    bool TrySend(const RStream& stream, unsigned priority = 1);
    /*
     *  params:
     *      buf ������Ϣ�Ļ���
     *      masmsgsize ��Ϣ����󳤶�,����POSIX��Ч��ֻ���SYSTEM V��Ч
     *      priority ��Ϣ���ͣ���������ʶ�ĸ��ͻ���;���Ϊ0���򲻹���Ϣ���ͣ�ֻȡ��һ����Ϣ
     *               ����POSIX��Ϣ������Ч��ʼ�շ���������ȼ������ͣ���Ϣ
     *  return:
     *      -1  ������Ϣ����,���maxmsgsizeС����Ϣ�ĳ��ȣ�Ҳ�����ش���;
     *      ȡ�õ���Ϣ���ȣ�û����Ϣ������.
     */
    int Recv(char* buf, size_t maxmsgsize, unsigned priority = 0);

    int Recv(RStream& stream, unsigned priority = 0);

    int TryRecv(char* buf, size_t maxmsgsize, unsigned priority = 0);

    int TryRecv(RStream& stream, unsigned priority = 0);

    const RString& Name() const;

    /*
     *  ���ص�����Ϣ���еĳ���
     */
    int Size() const;

    bool Remove();
private:
    RMsgQueue(const RMsgQueue&);        //��ֹ��Ϣ���п���

    const RMsgQueue& operator=(const RMsgQueue&);   //��ֹ��ֵ

private:
    RString         m_name;

    struct RMsg_t*  m_msg;
};

}       /*namespace*/

#endif /* RMSGQUEUE_HPP_HEADER_INCLUDED */
