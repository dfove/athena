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
     *  创建条件变量
     *  params
     *      process 是否允许进程间共享条件变量(如，将条件放在共享内存中),
     *          windows中默认为线程和进程均共享条件变量
     *          linux/Solaris默认为线程间共享,进程间共享需要显示指定
     */
    RCondition(bool process = false);

    ~RCondition();

public:
    /* 
     *  CondLock():修改条件变量之前需获取条件变量使用的锁
     *  CondUnlock():修改条件变量之后需释放条件变量使用的锁
     *  Example 1 (Linux/Unix作为条件变量机制):
     *    thread1:
     *      int ret = 0;
     *      RCondition cond;
     *      cond.CondLock();
     *      //1、可在此处修改"条件"
     *      while ( "条件" == false)
     *          ret = cond.Wait();//或者其它等待信号函数,这些函数内部会先释放再获取锁, \
                                    不会造成Wait的时候，其它线程无法获取锁
     *      //2、也可在此处修改"条件"
     *      cond.CondUnlock();
     *      ......
     *
     *    thread2:
     *      cond.CondLock();
     *      //3、在获取锁后修改条件变量
     *      cond.Signal();//或者cond.Broadcast(); Signal()仅唤醒一个Wait(),Broadcast()唤醒所有Wait()
     *      cond.CondUnlock();
     *
     ****************************************************
     *
     *  Example 2 (Windows作为事件机制):
     *    thread1:
     *      Rcondition cond;
     *      cond.Wait();//或其它等待事件函数
     *      ......
     *
     *    thread2:
     *      cond.Signal();//windows下默认为广播类型,同cond.BroadCast(),唤醒所有Wait();
     *      //Any number of waiting threads, \
              or threads that subsequently begin wait operations for the specified event object, \
              can be released while the object's state is signaled.
     */
    bool CondLock();

    bool CondUnlock();

    bool Wait();

    /*
     *  return value:
     *      -1  条件变量(事件)无效
     *      0   成功
     *      1   超时
     *      其它错误 同SetEvent()或pthread_cond_timedwait()或cond_timedwait()
     *  params:
     *      milliseconds 毫秒
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
