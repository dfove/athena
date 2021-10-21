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

#ifndef RSHAREMEM_HPP_HEADER_INCLUDED
#define RSHAREMEM_HPP_HEADER_INCLUDED

#include <Unidef.hpp>
#include <RString.hpp>

namespace athena
{
struct RShm_t;

class ATHENA_EX RShareMem
{
public:
    /*
     *  ���������ڴ�
     *  params:
     *      name    �ļ���
     *      size    �����ڴ��С�����ֽ�Ϊ��λ
     *              ���sizeΪ0��
     *                  ���������ڴ棬��Ĭ��Ϊ�ļ�ӳ���С��һ�����ڽ��̹���
     *                  ���������ڴ棬��Ĭ��Ϊһ���ֽڴ�С��һ�������̹߳���
     * blfilemap    �Ƿ��ļ�ӳ��,����SYSTEM V��Ч
     */
    RShareMem(const char* name = NULL, unsigned int size = 0, bool blfilemap = true);

    virtual ~RShareMem();

    void* GetShm() const;

    const RString& GetName() const;

    /*
     *  ��ϵͳɾ�������ڴ�ȥ�������(�����ڴ������ں˵ĳ�����)��
     */
    bool Remove();

private:
    RShareMem(const RShareMem&);

    const RShareMem& operator=(const RShareMem&);
    
private:
    struct RShm_t*  m_shm;

    bool            m_blfilemap;

    void*           m_mem;

    RString         m_name;
};  /*class RShareMem*/

}   /*namespace*/

#endif /*RSHAREMEM_HPP_HEADER_INCLUDED*/
