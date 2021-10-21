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

#ifndef RCONDITION_HPP_HEADER_INCLUDED
#define RCONDITION_HPP_HEADER_INCLUDED

#include <RTime.hpp>

namespace athena
{
struct RCond_t;
class RMutex;

class ATHENA_EX RCondition
{
public:
    /*
     *  ������������
     *  params
     *      process �Ƿ�������̼乲����������(�磬���������ڹ����ڴ���),
     *          windows��Ĭ��Ϊ�̺߳ͽ��̾�������������
     *          linux/SolarisĬ��Ϊ�̼߳乲��,���̼乲����Ҫ��ʾָ��
     */
    RCondition(bool process = false);

    ~RCondition();

public:
    /* 
     *  CondLock():�޸���������֮ǰ���ȡ��������ʹ�õ���
     *  CondUnlock():�޸���������֮�����ͷ���������ʹ�õ���
     *  Example 1 (Linux/Unix��Ϊ������������):
     *    thread1:
     *      int ret = 0;
     *      RCondition cond;
     *      cond.CondLock();
     *      //1�����ڴ˴��޸�"����"
     *      while ( "����" == false)
     *          ret = cond.Wait();//���������ȴ��źź���,��Щ�����ڲ������ͷ��ٻ�ȡ��, \
                                    �������Wait��ʱ�������߳��޷���ȡ��
     *      //2��Ҳ���ڴ˴��޸�"����"
     *      cond.CondUnlock();
     *      ......
     *
     *    thread2:
     *      cond.CondLock();
     *      //3���ڻ�ȡ�����޸���������
     *      cond.Signal();//����cond.Broadcast(); Signal()������һ��Wait(),Broadcast()��������Wait()
     *      cond.CondUnlock();
     *
     ****************************************************
     *
     *  Example 2 (Windows��Ϊ�¼�����):
     *    thread1:
     *      Rcondition cond;
     *      cond.Wait();//�������ȴ��¼�����
     *      ......
     *
     *    thread2:
     *      cond.Signal();//windows��Ĭ��Ϊ�㲥����,ͬcond.BroadCast(),��������Wait();
     *      //Any number of waiting threads, \
              or threads that subsequently begin wait operations for the specified event object, \
              can be released while the object's state is signaled.
     */
    bool CondLock();

    bool CondUnlock();

    bool Wait();

    /*
     *  return value:
     *      -1  ��������(�¼�)��Ч
     *      0   �ɹ�
     *      1   ��ʱ
     *      �������� ͬSetEvent()��pthread_cond_timedwait()��cond_timedwait()
     *  params:
     *      milliseconds ����
     */  
    int TimedWait(unsigned int milliseconds);

    bool Signal();

    bool Broadcast();

private:
    RCondition(const RCondition&);

    const RCondition& operator=(const RCondition&);

private:
    RCond_t*    m_cond;

    RMutex*     m_mutex;

};

} // namespace

#endif /* RCONDITION_HPP_HEADER_INCLUDED */
