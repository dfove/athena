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

#ifndef RFILELOCK_HPP_HEADER_INCLUDED
#define RFILELOCK_HPP_HEADER_INCLUDED

#include <Unidef.hpp>
#include <RString.hpp>

#include <unistd.h>
#include <fcntl.h>

namespace athena
{

class ATHENA_EX RFileLock
{
public:
    RFileLock(const RString& fileName, bool autoClose = true);

    RFileLock(int fd, bool autoClose = false);

    virtual ~RFileLock();

/*method*/
public:
    inline bool RdLock();
 
    inline bool TryRdLock();

    inline bool WrLock();

    inline bool TryWrLock();

    inline bool Unlock();

protected:
    /*
     *  params:
     *      cmd     F_SETLK F_SETLKW F_GETLK
     *      type    F_RDLCK F_WRLCK  F_UNLCK
     *      whence  SEEK_SET SEEK_CUR SEEK_END
     *      offset  the start point relative to whence
     *      len     the length of the locked bytes
     */
    int Lock_reg(int cmd, int type, int whence, off_t offset, off_t len);

private:
    int m_fd;

    bool m_autoClose;
};

class RSafeFRdLock
{
public:
    RSafeFRdLock(int fd):m_flock(fd)
    {
        m_flock.RdLock();
    }

    ~RSafeFRdLock()
    {
        m_flock.Unlock();
    }
private:
    RFileLock m_flock;
};

class RSafeFWrLock
{
public:
    RSafeFWrLock(int fd):m_flock(fd)
    {
        m_flock.WrLock();
    }

    ~RSafeFWrLock()
    {
        m_flock.Unlock();
    }
private:
    RFileLock m_flock;
};

inline bool 
RFileLock::RdLock()
{
    int ret = Lock_reg(F_SETLKW, F_RDLCK, SEEK_SET, 0, 0);
    return ret==-1?false:true;
}

inline bool 
RFileLock::TryRdLock()
{
    int ret = Lock_reg(F_SETLK, F_RDLCK, SEEK_SET, 0, 0);
    return ret==-1?false:true;
}

inline bool 
RFileLock::WrLock()
{
    int ret = Lock_reg(F_SETLKW, F_WRLCK, SEEK_SET, 0, 0);
    return ret==-1?false:true;
}

inline bool 
RFileLock::TryWrLock()
{
    int ret = Lock_reg(F_SETLK, F_WRLCK, SEEK_SET, 0, 0);
    return ret==-1?false:true;
}

inline bool 
RFileLock::Unlock()
{
    int ret = Lock_reg(F_SETLK, F_UNLCK, SEEK_SET, 0, 0);
    return ret==-1?false:true;
}

#define read_lock(fd,whence,offset,len)\
        lock_reg(fd,F_SETLK,F_RDLCK,whence,offset,len)
#define readw_lock(fd,whence,offset,len)\
        lock_reg(fd,F_SETLKW,F_RDLCK,whence,offset,len)
#define write_lock(fd,whence,offset,len)\
        lock_reg(fd,F_SETLK,F_WRLCK,whence,offset,len)
#define writew_lock(fd,whence,offset,len)\
        lock_reg(fd,F_SETLKW,F_WRLCK,whence,offset,len)
#define un_lock(fd,whence,offset,len)\
        lock_reg(fd,F_SETLK,F_UNLCK,whence,offset,len)

#ifdef __cplusplus
extern "C" {
#endif

int lock_reg(int fd, int cmd, int type, int whence, off_t offset, off_t len);

#ifdef __cplusplus
}
#endif

}   //namespace

#endif /*RFILELOCK_HPP_HEADER_INCLUDED*/
