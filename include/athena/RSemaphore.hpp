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

#ifndef RSEMAPHORE_HPP_HEADER_INCLUDED
#define RSEMAPHORE_HPP_HEADER_INCLUDED

#include <Unidef.hpp>
#include <RString.hpp>

namespace athena
{
struct RSem_t;

class ATHENA_EX RSemaphore
{
public:
    /*
     *  �����źŵ�
     *  params:
     *      name    �����źŵ�
     *      value   ��ʼ��ֵ
     */
    RSemaphore(const char* name = NULL, unsigned int value = 0);

    ~RSemaphore();

public:
    bool Wait();

    bool TryWait();

    bool Post();

    /*
     *  ȷ����û�еȴ����ź���������µ���
     */
    bool Remove();

    const RString& GetName() const;

    int  GetValue() const;

private:
    RSemaphore(const RSemaphore&);  //��ֹ�źŵƿ���

    const RSemaphore& operator=(const RSemaphore&); //��ֹ��ֵ

private:
    struct RSem_t   *m_sem;

    RString          m_name;
};

}

#endif /* RSEMAPHORE_HPP_HEADER_INCLUDE */
