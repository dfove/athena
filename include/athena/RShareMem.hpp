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
     *  创建共享内存
     *  params:
     *      name    文件名
     *      size    共享内存大小，以字节为单位
     *              如果size为0：
     *                  有名共享内存，则默认为文件映射大小，一般用于进程共享
     *                  无名共享内存，则默认为一个字节大小，一般用于线程共享
     * blfilemap    是否文件映射,对于SYSTEM V无效
     */
    RShareMem(const char* name = NULL, unsigned int size = 0, bool blfilemap = true);

    virtual ~RShareMem();

    void* GetShm() const;

    const RString& GetName() const;

    /*
     *  从系统删除共享内存去并拆除它(共享内存有随内核的持续性)；
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
