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

#ifndef RMUTEX_HPP_HEADER_INCLUDED
#define RMUTEX_HPP_HEADER_INCLUDED

#include <Unidef.hpp>

namespace athena
{

struct RMutex_t;

class ATHENA_EX RMutex
{
public:
    /*
     *  创建X锁，即互斥锁
     *  params
     *      process 是否允许进程间共享锁(如，将锁放在共享内存中),
     *          windows中默认为线程和进程均共享锁
     *          linux/Solaris默认为线程间共享,进程间共享需要显示指定
     */
    RMutex(bool process = false);

    virtual ~RMutex();

public:
    bool Lock();

    bool TryLock();

    bool Unlock();

private:
    RMutex(const RMutex&);                        //禁止锁拷贝

    const RMutex& operator =(const RMutex &);     //禁止赋值

private:
    friend class RCondition;

    struct RMutex_t *m_mut;
};

/****************************************************
 *  基于互斥锁的安全辅助锁,可以做到异常安全的解锁
 *　说明:     在创建类时加锁,析构时解锁;一种简单的但不是必须的使用方法是使用{}将要加锁的代码
 *            括起来，在{}中第一行定义该辅助类，在{}中使用该辅助类做相应的操作，}时如被锁定
 *            将自动解锁
****************************************************/
class ATHENA_EX RSafeXMutex
{
public:
    RSafeXMutex(RMutex& mut):m_mutex(mut)
    {
        m_mutex.Lock();
    }

    ~RSafeXMutex()
    {
        m_mutex.Unlock();
    }

private:
    RMutex& m_mutex;
};

#define XLOCK(mut) RSafeXMutex lock(mut)

/**************************************************************************************
* RSafe    基于各类锁的安全类模板
***************************************************************************************/
template<class _Tp,class _Mutex = RMutex>
class ATHENA_EX RSafe
{
public:
    typedef _Tp value_type;
    typedef _Mutex lock_type; 
    RSafe(void)
    {
    }

    ~RSafe(void)
    {
    }

    explicit RSafe(const _Tp& other)
    {
        m_Data = other;
    }

    RSafe(RSafe& other)
    {
        XLOCK(other.m_Mutex);
        m_Data = other.m_Data;
    }

    RSafe& operator=(RSafe& other)
    {
        if ( &other == this )
            return *this;
        XLOCK(other.m_Mutex);
        {
            XLOCK(m_Mutex);
            m_Data = other.m_Data;
        }
        return *this;
    }

    void SetValue(const _Tp& other)//设置值
    {
        XLOCK(m_Mutex);
        m_Data = other;
    }

    const _Tp GetValue(void)      //取得值,注意:不能像一般的取值一样做为const成员函数，因为加锁会改变其锁成员
    {
        XLOCK(m_Mutex);
        return m_Data;   
    }

    _Tp& Data(void)          //加锁后方可安全使用此函数得到资源的引用，如未加锁直接调用可能造成相应的安全问题
    {
        return m_Data;
    }

protected:
    _Tp m_Data;                 //数据成员
    _Mutex  m_Mutex;            //锁成员
};

}   /*namespace*/

#endif /* RMUTEX_HPP_HEADER_INCLUDE */
