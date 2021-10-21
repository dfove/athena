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

#ifndef RSMUTEX_HPP_HEADER_INCLUDED
#define RSMUTEX_HPP_HEADER_INCLUDED

#include <Unidef.hpp>

namespace athena
{

struct RSMutex_t;

class ATHENA_EX RSMutex
{
public:
    /*
     *  创建S锁，即读写锁
     *  params
     *      process 是否允许进程间共享锁(如，将锁放在共享内存中),
     *          windows中默认为线程和进程均共享锁
     *          linux/Solaris默认为线程间共享,进程间共享需要显示指定
     */
    RSMutex(bool process = false);

    ~RSMutex();

public:
    bool RdLock();
 
    bool TryRdLock();

    bool WrLock();

    bool TryWrLock();

    bool Unlock();

private:
    RSMutex(const RSMutex&);                        //禁止锁拷贝

    const RSMutex& operator =(const RSMutex &);     //禁止赋值

private:
    struct RSMutex_t*   m_rwlock;

    bool                m_bpshared;                 //是否进程间共享
};

/****************************************************
 *  基于读写锁的安全辅助锁,可以做到异常安全的解锁
 *　说明:     在创建类时加锁,析构时解锁;一种简单的但不是必须的使用方法是使用{}将要加锁的代码
 *            括起来，在{}中第一行定义该辅助类，在{}中使用该辅助类做相应的操作，}时如被锁定
 *            将自动解锁
****************************************************/
class ATHENA_EX RSafeSMutex
{
public:
    RSafeSMutex(RSMutex& mut, bool isRead):m_mutex(mut)
    {
        if ( isRead == true )
            m_mutex.RdLock();
        else
            m_mutex.WrLock();
    }

    ~RSafeSMutex()
    {
        m_mutex.Unlock();
    }

private:
    RSMutex& m_mutex;
};

#define RDLOCK(mut) RSafeSMutex lock(mut, true)
#define WRLOCK(mut) RSafeSMutex lock(mut, false)

/**************************************************************************************
* RSafeEx    基于读写锁的安全类模板
***************************************************************************************/
template<class _Tp,class _Mutex = RSMutex>
class ATHENA_EX RSafeEx
{
public:
    typedef _Tp value_type;
    typedef _Mutex lock_type; 
    RSafeEx(void)
    {
    }

    ~RSafeEx(void)
    {
    }

    explicit RSafeEx(const _Tp& other)
    {
        m_Data = other;
    }

    RSafeEx(RSafeEx& other)
    {
        RDLOCK(other.m_Mutex);
        m_Data = other.m_Data;
    }

    RSafeEx& operator=(RSafeEx& other)
    {
        RDLOCK(other.m_Mutex);
        {
            WRLOCK(m_Mutex);
            m_Data = other.m_Data;
        }
        return *this;
    }

    void SetValue(const _Tp& other)
    {
        WRLOCK(m_Mutex);
        m_Data = other;
    }

    const _Tp GetValue(void)
    {
        RDLOCK(m_Mutex);
        return m_Data;
    }

    _Tp& Data(void)
    {
        return m_Data;
    }

protected:
    _Tp m_Data;
    _Mutex  m_Mutex;
};

}   /*namespace*/

#endif /* RMUTEX_HPP_HEADER_INCLUDE */
