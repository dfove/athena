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
     *  ����X������������
     *  params
     *      process �Ƿ�������̼乲����(�磬�������ڹ����ڴ���),
     *          windows��Ĭ��Ϊ�̺߳ͽ��̾�������
     *          linux/SolarisĬ��Ϊ�̼߳乲��,���̼乲����Ҫ��ʾָ��
     */
    RMutex(bool process = false);

    virtual ~RMutex();

public:
    bool Lock();

    bool TryLock();

    bool Unlock();

private:
    RMutex(const RMutex&);                        //��ֹ������

    const RMutex& operator =(const RMutex &);     //��ֹ��ֵ

private:
    friend class RCondition;

    struct RMutex_t *m_mut;
};

/****************************************************
 *  ���ڻ������İ�ȫ������,���������쳣��ȫ�Ľ���
 *��˵��:     �ڴ�����ʱ����,����ʱ����;һ�ּ򵥵ĵ����Ǳ����ʹ�÷�����ʹ��{}��Ҫ�����Ĵ���
 *            ����������{}�е�һ�ж���ø����࣬��{}��ʹ�øø���������Ӧ�Ĳ�����}ʱ�类����
 *            ���Զ�����
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
* RSafe    ���ڸ������İ�ȫ��ģ��
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

    void SetValue(const _Tp& other)//����ֵ
    {
        XLOCK(m_Mutex);
        m_Data = other;
    }

    const _Tp GetValue(void)      //ȡ��ֵ,ע��:������һ���ȡֵһ����Ϊconst��Ա��������Ϊ������ı�������Ա
    {
        XLOCK(m_Mutex);
        return m_Data;   
    }

    _Tp& Data(void)          //�����󷽿ɰ�ȫʹ�ô˺����õ���Դ�����ã���δ����ֱ�ӵ��ÿ��������Ӧ�İ�ȫ����
    {
        return m_Data;
    }

protected:
    _Tp m_Data;                 //���ݳ�Ա
    _Mutex  m_Mutex;            //����Ա
};

}   /*namespace*/

#endif /* RMUTEX_HPP_HEADER_INCLUDE */
