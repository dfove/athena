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

#include <RStream.hpp>

using namespace athena;

RStream::RStream()
        :m_size(0)
        ,m_capacity(MIN_CAPACITY)
        ,m_buf(0)
{
    m_buf = new char[m_capacity];
    memset(m_buf, 0, m_capacity);
}

RStream::RStream(size_t capacity)
        :m_size(0)
        ,m_capacity(capacity)
        ,m_buf(0)
{
    m_buf = new char[m_capacity];
    memset(m_buf, 0, m_capacity);
}

RStream::RStream(const char* buf, size_t count)
        :m_size(count)
        ,m_capacity(MIN_CAPACITY)
        ,m_buf(0)
{
    if ( count == RStream::npos )
        m_size = strlen(buf);
    m_capacity = ComputeCapacity(m_size);
    m_buf = new char[m_capacity];
    memset(m_buf, 0, m_capacity);
    if ( buf )
        memmove(m_buf, buf, m_size);
}

RStream::RStream(const void* buf, size_t count)
        :m_size(count)
        ,m_capacity(MIN_CAPACITY)
        ,m_buf(0)
{
    m_capacity = ComputeCapacity(m_size);
    m_buf = new char[m_capacity];
    memset(m_buf, 0, m_capacity);
    if ( buf )
        memmove(m_buf, buf, m_size);
}

RStream::RStream(const RStream& rstream)
        :m_size(rstream.m_size)
        ,m_capacity(rstream.m_capacity)
        ,m_buf(0)
{
    m_buf = new char[m_capacity];
    memmove(m_buf, rstream.m_buf, m_size);
}

RStream::~RStream()
{
    if ( m_buf )
    {
        delete[] m_buf;
        m_buf = NULL;
    }
}

/*operator*/
RStream& 
RStream::operator=(const char* rstr)
{
    if ( !rstr )
        return *this;
    return Assign(rstr, strlen(rstr));
}

RStream& 
RStream::operator=(const RStream& rstream)
{
    return Assign(rstream);
}


RStream& 
RStream::operator+=(const RStream& rstream)
{
    return Append(rstream);
}


RStream& 
RStream::operator<<(const RStream& rstream)
{
    return Append(rstream);
}


RStream& 
RStream::operator<<(int value)
{
    return Append(&value, sizeof(int));
}


RStream& 
RStream::operator<<(unsigned value)
{
    return Append(&value, sizeof(int));
}


RStream& 
RStream::operator<<(char value)
{
    return Append(1, value);
}

namespace athena
{

const RStream 
operator+=(const RStream& lstream, const RStream& rstream)
{
    RStream ret;
    ret.Append(lstream);
    ret.Append(rstream);
    return ret;
}

bool 
operator>(const RStream& lstream, const RStream& rstream)
{
    return lstream.Compare(rstream)>0?true:false;
}

bool 
operator==(const RStream& lstream, const RStream& rstream)
{
    return lstream.Compare(rstream)==0?true:false;
}

bool 
operator!=(const RStream& lstream, const RStream& rstream)
{
    return lstream.Compare(rstream)!=0?true:false;
}

bool 
operator<(const RStream& lstream, const RStream& rstream)
{
    return lstream.Compare(rstream)<0?true:false;
}

}   /*namespace*/

/*member functions*/
RStream& 
RStream::Append(const char* buf)
{
    return Append(buf, strlen(buf));
}

RStream& 
RStream::Append(const void* buf, size_t count)
{
    if ( !buf || count == 0 )
        return *this;
    size_t savesize = m_size;
    Resize(m_size + count);
    memmove(m_buf + savesize, buf, count);
    return *this;
}

RStream& 
RStream::Append(const RStream& rstream)
{
    return Append(rstream.m_buf, rstream.m_size);
}

RStream& 
RStream::Append(const RStream& rstream, unsigned off, size_t count)
{
    if ( off >= rstream.m_size )
        return *this;
    size_t nappend = Min(rstream.m_size - off, count);
    return Append(rstream.m_buf, nappend);
}

RStream& 
RStream::Append(size_t count, char chr)
{
    if ( count != 0 )
    {
        size_t savesize = m_size;
        Resize(m_size + count);
        for ( unsigned i = 0; i < count; i++ )
        {
            *(m_buf+savesize+i) = chr;
        }
    }
    return *this;
}

RStream& 
RStream::Assign(const void* buf, size_t count)
{
    Clear();
    Append(buf, count);
    return *this;
}

RStream& 
RStream::Assign(const RStream& rstream, unsigned off, size_t count)
{
    if ( this == &rstream )
        return *this;

    Clear();
    if ( off >= rstream.m_size )
    {
        return *this;
    }
    size_t ncopy = Min(rstream.m_size - off, count);
    Append(rstream.Data() + off, ncopy);
    return *this;
}

RStream& 
RStream::Assign(const RStream& rstream)
{
    if ( this != &rstream )
    {
        Assign(rstream, 0, rstream.Size());
    }
    return *this;
}

RStream& 
RStream::Assign(size_t count)
{
    if ( m_buf )
    {
        delete[] m_buf;
    }
    m_size = count;
    m_capacity = ComputeCapacity(m_size);
    m_buf = new char[m_capacity];
    memset(m_buf, 0, m_capacity);
    return *this;
}

size_t 
RStream::Capacity() const
{
    return m_capacity;
}

void 
RStream::Clear()
{
//    if ( m_buf )
//    {
//        delete[] m_buf;
//    }
    m_size = 0;
//    m_capacity = MIN_CAPACITY;
//    m_buf = new char[m_capacity];
    memset(m_buf, 0, m_capacity);
}

int 
RStream::Compare(const RStream& rstream) const
{
    return Compare(0, m_size, rstream);
}

int 
RStream::Compare(unsigned off1, size_t num1, const RStream& rstream) const
{
    return Compare(off1, num1, rstream, 0, rstream.Size());
}

int 
RStream::Compare(unsigned off1, size_t num1, const RStream& rstream, unsigned off2, size_t num2) const
{
    size_t ncmp = 0, ncmp1 = 0, ncmp2 = 0;
    if ( off1 >= m_size || num1 == 0 )
    {
        if ( off2 >= rstream.m_size || num2 == 0 )
        {
            return 0;
        }
        else
            return -1;
    }
    else
    {
        if ( off2 >= rstream.m_size || num2 == 0 )
            return 1;
        else
        {
            ncmp1 = Min(m_size - off1, num1);
            ncmp2 = Min(rstream.m_size - off2, num2);
            ncmp = Min(ncmp1, ncmp2);
            int nret =  memcmp(m_buf + off1, rstream.m_buf + off2, ncmp);
            if ( nret == 0 && ncmp1 != ncmp2 )
            {
                return ncmp1>ncmp2?1:-1;
            }
            return nret;
        }
    }
}

int 
RStream::Compare(const void* buf, size_t num2) const
{
    return Compare(0, Size(), buf, 0, num2);
}

int 
RStream::Compare(unsigned off1, size_t num1, const void* buf, unsigned off2, size_t num2) const
{
    size_t ncmp = 0, ncmp1 = 0;
    if ( buf )
    {
        if (  off1 >= m_size || num1 == 0  )
        {
            if ( num2 == 0)
                return 0;
            else
                return -1;
        }
        else
        {
            if ( num2 == 0 )
                return 1;
            else
            {
                ncmp1 = Min(m_size - off1, num1);
                ncmp = Min(ncmp1, num2);
                int nret = memcmp(m_buf + off1, (char*)buf + off2, ncmp);
                if ( nret == 0 && ncmp1 != num2 )
                {
                    return ncmp1>num2?1:-1;
                }
                return nret;
            }
        }
    }
    else
    {
        if ( off1 >= m_size || num1 == 0 )
            return 0;
        else
        {
            return 1;
        }
    }
}

size_t 
RStream::Copy(void* buf, size_t count, unsigned off) const
{
    if ( !buf || off >= m_size  )
        return 0;
    size_t ncopy = Min(m_size - off, count);
    memmove(buf, m_buf + off, ncopy);
    return ncopy;
}

const char* 
RStream::Data() const
{
    return m_buf;
}

char* 
RStream::Data()
{
    return m_buf;
}

void 
RStream::SetSize(size_t size)
{
    if ( size > m_capacity )
        m_size = m_capacity;
    m_size = size;
}

bool 
RStream::Empty() const
{
    return (m_size == 0)?true:false;
}

RStream& 
RStream::Erase(unsigned off, size_t count)
{
    if ( off < m_size )
    {
        if ( off == 0 && count == m_size )
            Clear();
        else
        {
            size_t nerase = Min(m_size - off, count);
            memmove(m_buf + off, m_buf + off + nerase, m_size - off - nerase);
            memset(m_buf + m_size - nerase, 0, nerase);
            m_size = m_size - nerase;
        }
    }
    return *this;
}

RStream& 
RStream::Insert(unsigned off1, const void* buf, size_t count, unsigned off2)
{
    if ( off1 > m_size )
        return *this;
    else if( off1 == m_size )
    {
        Append((char*)buf + off2, count);
    }
    else
    {
        size_t savesize = m_size;
        Resize(m_size + count);
        memmove(m_buf + off1 + count, m_buf + off1, savesize - off1);
        memmove(m_buf + off1, (char*)buf + off2, count);
    }
    return *this;
}

RStream& 
RStream::Insert(unsigned off1, const RStream& rstream)
{
    return Insert(off1, rstream.m_buf, rstream.m_size, 0);
}

RStream& 
RStream::Insert(unsigned off1, const RStream& rstream, unsigned off2,  size_t count2)
{
    if ( off2 >= rstream.m_size )
        return *this;
    size_t count = Min(rstream.m_size - off2, count2);
    return Insert(off1, rstream.m_buf, count, off2);
}

size_t 
RStream::Length() const
{
    return m_size;
}

RStream& 
RStream::Replace(unsigned off1, size_t num1, const char* buf)
{
    return Replace(off1, num1, buf, 0, strlen(buf));
}

RStream& 
RStream::Replace(unsigned off1, const RStream& rstream)
{
    return Replace(off1, m_size, rstream);
}

RStream& 
RStream::Replace(unsigned off1, size_t num1, const RStream& rstream)
{
    return Replace(off1, num1, rstream.m_buf, 0, rstream.m_size);
}

RStream& 
RStream::Replace(unsigned off1, size_t num1, const void* buf, unsigned off2, size_t num2)
{
    if ( off1 >= m_size )
        return *this;
    size_t nrpl = Min(m_size - off1, num1);
    nrpl = Min(nrpl, num2);
    memmove(m_buf + off1, (char*)buf + off2, nrpl);
    return *this;
}

RStream& 
RStream::Replace(unsigned off1, size_t num1, const RStream& rstream, unsigned off2, size_t num2)
{
    if ( off2 >= rstream.m_size )
        return *this;
    return Replace(off1, num1, rstream.m_buf, off2, num2);
}

RStream& 
RStream::Replace(unsigned off, size_t num, char chr)
{
    if ( off >= m_size )
        return *this;
    size_t nrpl = Min(m_size - off, num);
    for (unsigned i = 0; i < nrpl; i++)
    {
        *(m_buf+off+i) = chr;
    }
    return *this;
}

void 
RStream::Resize(size_t count, char chr)
{
    if ( m_size == count )
        return;
    char* savebuf = m_buf;
    size_t savesize = m_size;
    size_t capacity = 0;

    m_size = count;
    capacity = ComputeCapacity(m_size);

    if ( capacity > m_capacity )
    {
        m_capacity = capacity;
        m_buf = new char[m_capacity];
        memset(m_buf, 0, m_capacity);
        memmove(m_buf, savebuf, Min(m_size, savesize));
        delete[] savebuf;
    }
    if ( m_size > savesize )
    {
        for ( size_t i = 0; i < m_size - savesize; i++ )
        {
            *(m_buf+savesize+i) = chr;
        }
    }
}

size_t 
RStream::Size() const
{
    return m_size;
}

RStream 
RStream::Substr(unsigned off, size_t count) const
{
    RStream stream;
    if ( off >= m_size )
        return stream;
    if ( off < 0 )
        off = 0;
    size_t ncopy = 0;
    if ( count == npos )
        ncopy = m_size - off;
    else
        ncopy = Min(m_size - off, count);

    stream.Assign(m_buf+off, ncopy);
    return stream;
}

size_t 
RStream::ComputeCapacity(size_t size) const
{
    if ( size <= m_capacity )
        return m_capacity;
    size_t capacity = MIN_CAPACITY;
    while ( capacity < size )
    {
        capacity = (size_t)(capacity*1.5);
    }
    return capacity;
}
