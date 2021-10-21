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

#include <RFileLock.hpp>
#include <RString.hpp>

using namespace athena;

RFileLock::RFileLock(const RString& fileName, bool autoClose /*= true*/)
{
    m_fd = open(fileName.c_str(), O_RDONLY);
    m_autoClose = autoClose;
}

RFileLock::RFileLock(int fd, bool autoClose /*= false*/)
          :m_fd(fd),
           m_autoClose(autoClose)
{
    
}

RFileLock::~RFileLock()
{
    if ( m_fd != -1 && m_autoClose == true )
        close(m_fd);
}

/*
bool 
RFileLock::RdLock()
{
    int ret = Lock_reg(F_SETLKW, F_RDLCK, 0, SEEK_SET, 0);
    return ret==-1?false:true;
}
 
bool 
RFileLock::TryRdLock()
{
    int ret = Lock_reg(F_SETLK, F_RDLCK, 0, SEEK_SET, 0);
    return ret==-1?false:true;
}

bool 
RFileLock::WrLock()
{
    int ret = Lock_reg(F_SETLKW, F_WRLCK, 0, SEEK_SET, 0);
    return ret==-1?false:true;
}

bool 
RFileLock::TryWrLock()
{
    int ret = Lock_reg(F_SETLK, F_WRLCK, 0, SEEK_SET, 0);
    return ret==-1?false:true;
}

bool 
RFileLock::Unlock()
{
    int ret = Lock_reg(F_SETLK, F_UNLCK, 0, SEEK_SET, 0);
    return ret==-1?false:true;
}
*/

int 
RFileLock::Lock_reg(int cmd, int type, int whence, off_t offset, off_t len)
{
   struct flock lock_it;
   
   lock_it.l_type = type;
   lock_it.l_whence = whence;
   lock_it.l_start = offset;
   lock_it.l_len = len;
   return (fcntl(m_fd,cmd,&lock_it));
}

int lock_reg(int fd,int cmd,int type,off_t offset,int whence,off_t len)
{
   struct flock lock_it;
   
   lock_it.l_type = type;
   lock_it.l_whence = whence;
   lock_it.l_start = offset;
   lock_it.l_len = len;
   return (fcntl(fd,cmd,&lock_it));
}
