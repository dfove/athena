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
     *  ����S��������д��
     *  params
     *      process �Ƿ�������̼乲����(�磬�������ڹ����ڴ���),
     *          windows��Ĭ��Ϊ�̺߳ͽ��̾�������
     *          linux/SolarisĬ��Ϊ�̼߳乲��,���̼乲����Ҫ��ʾָ��
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
    RSMutex(const RSMutex&);                        //��ֹ������

    const RSMutex& operator =(const RSMutex &);     //��ֹ��ֵ

private:
    struct RSMutex_t*   m_rwlock;

    bool                m_bpshared;                 //�Ƿ���̼乲��
};

/****************************************************
 *  ���ڶ�д���İ�ȫ������,���������쳣��ȫ�Ľ���
 *��˵��:     �ڴ�����ʱ����,����ʱ����;һ�ּ򵥵ĵ����Ǳ����ʹ�÷�����ʹ��{}��Ҫ�����Ĵ���
 *            ����������{}�е�һ�ж���ø����࣬��{}��ʹ�øø���������Ӧ�Ĳ�����}ʱ�类����
 *            ���Զ�����
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
* RSafeEx    ���ڶ�д���İ�ȫ��ģ��
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
