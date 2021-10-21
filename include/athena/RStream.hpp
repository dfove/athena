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

#ifndef RSTREAM_HPP_HEADER_INCLUDED
#define RSTREAM_HPP_HEADER_INCLUDED

#include <Unidef.hpp>
#include <limits.h>
#include <string.h>

namespace athena
{

class ATHENA_EX RStream
{
public:
    RStream();

    explicit RStream(size_t capacity);

    explicit RStream(const char* buf, size_t count = RStream::npos);

    RStream(const void* buf, size_t count);

    RStream(const RStream& rstream);

    virtual ~RStream();

/*operation*/
public:
    /*
     *  param:
     *      rstr, must is terminated by '\0'
     */
    RStream& operator=(const char* rstr);

    RStream& operator=(const RStream& rstream);

    RStream& operator+=(const RStream& rstream);

    RStream& operator<<(const RStream& rstream);

    RStream& operator<<(int value);

    RStream& operator<<(unsigned value);

    RStream& operator<<(char value);

    ATHENA_EX friend const RStream operator+(const RStream& lstream, const RStream& rstream);

    ATHENA_EX friend bool operator>(const RStream& lstream, const RStream& rstream);

    ATHENA_EX friend bool operator==(const RStream& lstream, const RStream& rstream);

    ATHENA_EX friend bool operator!=(const RStream& lstream, const RStream& rstream);

    ATHENA_EX friend bool operator<(const RStream& lstream, const RStream& rstream);

/*member functions*/
public:
    RStream& Append(const char* buf);

    RStream& Append(const void* buf, size_t count);

    RStream& Append(const RStream& rstream);

    RStream& Append(const RStream& rstream, unsigned off, size_t count);

    RStream& Append(size_t count, char chr);

    RStream& Assign(const void* buf, size_t count);

    RStream& Assign(const RStream& rstream, unsigned off, size_t count);

    RStream& Assign(const RStream& rstream);

    RStream& Assign(size_t count);

    size_t Capacity() const;

    void Clear();

    /*
     *  A negative value if the operand string is less than the parameter string; 
     *  zero if the two strings are equal; 
     *  or a positive value if the operand string is greater than the parameter string.
     */
    int Compare(const RStream& rstream) const;

    int Compare(unsigned off1, size_t num1, const RStream& rstream) const;

    int Compare(unsigned off1, size_t num1, const RStream& rstream, unsigned off2, size_t num2) const;

    int Compare(const void* buf, size_t num2) const;

    int Compare(unsigned off1, size_t num1, const void* buf, unsigned off2, size_t num2) const;

    size_t Copy(void* buf, size_t count, unsigned off = 0) const;

    const char* Data() const;

    char* Data();

    void SetSize(size_t size);

    bool Empty() const;

    RStream& Erase(unsigned off, size_t count);

    RStream& Insert(unsigned off1, const void* buf, size_t count, unsigned off2 = 0);

    RStream& Insert(unsigned off1, const RStream& rstream);

    RStream& Insert(unsigned off1, const RStream& rstream, unsigned off2, size_t count2);

    size_t Length() const;

    /*
     *  params:
     *      off1: The index of the operand string at which the replacement begins. 
     *      num1: The maximum number of characters to be replaced in the operand string. 
     *      buf:  The C-string that is to be a source of characters for the operand string.
     *      off2: The index of the parameter string at which the copying begins. 
     *      num2: The maximum number of characters to be used from the parameter C-string. 
     */
    RStream& Replace(unsigned off1, size_t num1, const char* buf);

    RStream& Replace(unsigned off1, const RStream& rstream);

    RStream& Replace(unsigned off1, size_t num1, const RStream& rstream);

    RStream& Replace(unsigned off1, size_t num1, const void* buf, unsigned off2, size_t num2);

    RStream& Replace(unsigned off1, size_t num1, const RStream& rstream, unsigned off2, size_t num2);

    RStream& Replace(unsigned off, size_t num, char chr);

    void Resize(size_t count, char chr = '\0');

    size_t Size() const;

    RStream Substr(unsigned off = 0, size_t count = npos) const;

protected:
    /*size: m_size*/
    size_t ComputeCapacity(size_t size) const;

private:
    /*
     *  in vc6 will be error
     */
    static const size_t npos = UINT_MAX;

    static const size_t  MIN_CAPACITY = 2;

    size_t      m_size;

    size_t      m_capacity;

    char*       m_buf;
};

}   /*namespace*/

#endif  /*RSTREAM_HPP_HEADER_INCLUDED*/
