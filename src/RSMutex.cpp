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

#include <RSMutex.hpp>

//namespace athena
//{
#if defined (WIN32) || defined (_WINDOWS)
#ifndef _WIN32_WINNT
#error "compile TryEnterCriticalSection should define the _WIN32_WINNT macro as 0x0400 or later"
#endif
    #include <windows.h>
    #include <limits.h>
    typedef struct
    {
        int rw_refCount;    // -1 writer has the lock, 0 is valid, >0 readers holding the lock
        HANDLE rw_unlockEvent;
        union 
        {
            HANDLE              accessMutex;
            CRITICAL_SECTION    csAccess;
        }rw_accessMutex;
        union
        {
            HANDLE              stateMutex;
            CRITICAL_SECTION    csState;
        }rw_stateChangeMutex;
    }
    RwLock_t;
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H) || \
	  defined(HAVE_PTHREAD_WIN32)
    #include <pthread.h>
    typedef pthread_rwlock_t RwLock_t;
#elif defined(HAVE_THREAD_H)   //solaris
    #include <thread.h>
    typedef rwlock_t RwLock_t;
#endif

//}

using namespace athena;

RSMutex::RSMutex(bool process /*= false*/)
              :m_rwlock(NULL),
               m_bpshared(process)
{
    RwLock_t *rwlock = NULL;
    rwlock = new RwLock_t;
#if defined (WIN32) || defined (_WINDOWS)
    rwlock->rw_refCount = 0;
    if ( (rwlock->rw_unlockEvent = CreateEvent(NULL, TRUE, FALSE, NULL)) == NULL )
    {
        free (m_rwlock);
        m_rwlock = NULL;
    }
    if ( m_bpshared == true )
    {
        if((rwlock->rw_accessMutex.accessMutex = CreateMutex(NULL, FALSE, NULL)) == NULL)
        {
            CloseHandle(rwlock->rw_unlockEvent);
            free (m_rwlock);
            m_rwlock = NULL;
        }
        if ( (rwlock->rw_stateChangeMutex.stateMutex = CreateMutex(NULL, FALSE, NULL)) == NULL )
        {
            CloseHandle(rwlock->rw_unlockEvent);
            CloseHandle(rwlock->rw_accessMutex.accessMutex);
            free (m_rwlock);
            m_rwlock = NULL;
        }
    }
    else
    {
        InitializeCriticalSection(&rwlock->rw_accessMutex.csAccess);
        InitializeCriticalSection(&rwlock->rw_stateChangeMutex.csState);
    }
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
	pthread_rwlockattr_t rwlockattr;
	pthread_rwlockattr_init( &rwlockattr );
	if ( m_bpshared == true )  //否则默认为 PTHREAD_PROCESS_PRIVATE
	    pthread_rwlockattr_setpshared(&rwlockattr, PTHREAD_PROCESS_SHARED);
    pthread_rwlock_init( rwlock, &rwlockattr );
    pthread_rwlockattr_destroy( &rwlockattr );
#elif defined(HAVE_THREAD_H)   //solaris
    int type = 0;
    if ( m_bpshared == true )
        type |= USYNC_PROCESS;
    rwlock_init(rwlock, type, NULL);
#endif
    m_rwlock = (struct RSMutex_t *)rwlock;
}

RSMutex::~RSMutex()
{
    if(m_rwlock != NULL)
    {
        RwLock_t * rwlock = (RwLock_t*)m_rwlock;
#if defined (WIN32) || defined (_WINDOWS)
        rwlock->rw_refCount = 0;
        CloseHandle(rwlock->rw_unlockEvent);
        if ( m_bpshared == TRUE )
        {
            CloseHandle(rwlock->rw_accessMutex.accessMutex);
            CloseHandle(rwlock->rw_stateChangeMutex.stateMutex);
        }
        else
        {
            DeleteCriticalSection(&rwlock->rw_accessMutex.csAccess);
            DeleteCriticalSection(&rwlock->rw_stateChangeMutex.csState);
        }
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
        pthread_rwlock_destroy(rwlock);
#elif defined(HAVE_THREAD_H)   //solaris
        rwlock_destroy(rwlock);
#endif
        delete rwlock;
        rwlock = NULL;
        m_rwlock = NULL;
    }
}

bool 
RSMutex::RdLock()
{
    if(m_rwlock == NULL)
        return false;

    RwLock_t * rwlock = (RwLock_t*)m_rwlock;
#if defined (WIN32) || defined (_WINDOWS)
    DWORD waitResult;
    bool bResult = true;
    if ( m_bpshared == true )
    {
        //使用读写锁的权限
        waitResult = WaitForSingleObject(rwlock->rw_accessMutex.accessMutex, INFINITE);
        if ( waitResult != WAIT_OBJECT_0 )
            return false;
        if ( rwlock->rw_refCount >= 0 )     //读写锁还没有被锁定,或只有读锁
        {
            //改变rw_refcount状态的权限
            waitResult = WaitForSingleObject(rwlock->rw_stateChangeMutex.stateMutex, INFINITE);
            if ( waitResult == WAIT_OBJECT_0 )
            {
                rwlock->rw_refCount++;
                ResetEvent(rwlock->rw_unlockEvent); //设置为nosignal状态，让其它进程(或线程)等
                ReleaseMutex(rwlock->rw_stateChangeMutex.stateMutex);
            }
            else
                bResult = false;
        }
        else    //已经被写锁定，则等待写锁被释放
        {
            waitResult = WaitForSingleObject(rwlock->rw_unlockEvent, INFINITE);
            if ( waitResult == WAIT_OBJECT_0 )      //写锁已释放
            {
                waitResult = WaitForSingleObject(rwlock->rw_stateChangeMutex.stateMutex, INFINITE);
                if ( waitResult == WAIT_OBJECT_0 )
                {
                    rwlock->rw_refCount++;
                    ResetEvent(rwlock->rw_unlockEvent);
                    ReleaseMutex(rwlock->rw_stateChangeMutex.stateMutex);
                }
                else 
                    bResult = false;
            }
            else
                bResult = false;
        }
        ReleaseMutex(rwlock->rw_accessMutex.accessMutex);
    }
    else
    {
        EnterCriticalSection(&rwlock->rw_accessMutex.csAccess);
        if ( rwlock->rw_refCount >= 0 )     //读写锁还没有被锁定,或只有读锁
        {
            EnterCriticalSection(&rwlock->rw_stateChangeMutex.csState);
            rwlock->rw_refCount++;
            ResetEvent(rwlock->rw_unlockEvent);
            LeaveCriticalSection(&rwlock->rw_stateChangeMutex.csState);
        }
        else    //已经被写锁定，则等待写锁被释放
        {
            waitResult = WaitForSingleObject(rwlock->rw_unlockEvent, INFINITE);
            if ( waitResult == WAIT_OBJECT_0 )
            {
                EnterCriticalSection(&rwlock->rw_stateChangeMutex.csState);
                rwlock->rw_refCount++;
                ResetEvent(rwlock->rw_unlockEvent);
                LeaveCriticalSection(&rwlock->rw_stateChangeMutex.csState);
            }
            else
                bResult = false;
        }
        LeaveCriticalSection(&rwlock->rw_accessMutex.csAccess);
    }
    return bResult;
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    int ret = pthread_rwlock_rdlock(rwlock);
    if ( ret != 0 )
        return false;
#else
    int ret = rw_rdlock(rwlock);
    if ( ret != 0 )
        return false;
#endif
    return true;
}

bool 
RSMutex::TryRdLock()
{
    if(m_rwlock == NULL)
        return false;

    RwLock_t * rwlock = (RwLock_t*)m_rwlock;
#if defined (WIN32) || defined (_WINDOWS)
    DWORD waitResult;
    bool bResult = true;
    if ( m_bpshared == true )
    {
        //使用读写锁的权限
        waitResult = WaitForSingleObject(rwlock->rw_accessMutex.accessMutex, 0);
        if ( waitResult != WAIT_OBJECT_0 )
            return false;
        if ( rwlock->rw_refCount >= 0 )     //读写锁还没有被锁定,或只有读锁
        {
            //改变rw_refcount状态的权限
            waitResult = WaitForSingleObject(rwlock->rw_stateChangeMutex.stateMutex, 0);
            if ( waitResult == WAIT_OBJECT_0 )
            {
                rwlock->rw_refCount++;
                ResetEvent(rwlock->rw_unlockEvent); //设置为nosignal状态，让其它进程(或线程)等
                ReleaseMutex(rwlock->rw_stateChangeMutex.stateMutex);
            }
            else
                bResult = false;
        }
        else    //已经被写锁定，则等待写锁被释放
        {
            waitResult = WaitForSingleObject(rwlock->rw_unlockEvent, 0);
            if ( waitResult == WAIT_OBJECT_0 )      //写锁已释放
            {
                waitResult = WaitForSingleObject(rwlock->rw_stateChangeMutex.stateMutex, 0);
                if ( waitResult == WAIT_OBJECT_0 )
                {
                    rwlock->rw_refCount++;
                    ResetEvent(rwlock->rw_unlockEvent);
                    ReleaseMutex(rwlock->rw_stateChangeMutex.stateMutex);
                }
                else 
                    bResult = false;
            }
            else
                bResult = false;
        }
        ReleaseMutex(rwlock->rw_accessMutex.accessMutex);
    }
    else
    {
        if ( TryEnterCriticalSection(&rwlock->rw_accessMutex.csAccess) == FALSE )
            return false;
        if ( rwlock->rw_refCount >= 0 )     //读写锁还没有被锁定,或只有读锁
        {
            if ( TryEnterCriticalSection(&rwlock->rw_stateChangeMutex.csState) == TRUE )
            {
                rwlock->rw_refCount++;
                ResetEvent(rwlock->rw_unlockEvent);
                LeaveCriticalSection(&rwlock->rw_stateChangeMutex.csState);
            }
            else
                bResult = false;
        }
        else    //已经被写锁定，则等待写锁被释放
        {
            waitResult = WaitForSingleObject(rwlock->rw_unlockEvent, 0);
            if ( waitResult == WAIT_OBJECT_0 )
            {
                EnterCriticalSection(&rwlock->rw_stateChangeMutex.csState);
                rwlock->rw_refCount++;
                ResetEvent(rwlock->rw_unlockEvent);
                LeaveCriticalSection(&rwlock->rw_stateChangeMutex.csState);
            }
            else
                bResult = false;
        }
        LeaveCriticalSection(&rwlock->rw_accessMutex.csAccess);
    }
    return bResult;
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    int ret = pthread_rwlock_tryrdlock(rwlock);
    if ( ret != 0 )
        return false;
#else
    int ret = rw_tryrdlock(rwlock);
    if ( ret != 0 )
        return false;
#endif
    return true;
}

bool 
RSMutex::WrLock()
{
    if(m_rwlock == NULL)
        return false;

    RwLock_t * rwlock = (RwLock_t*)m_rwlock;
#if defined (WIN32) || defined (_WINDOWS)
    DWORD waitResult;
    bool bResult = true;
    if ( m_bpshared == true )
    {
        //使用读写锁的权限
        waitResult = WaitForSingleObject(rwlock->rw_accessMutex.accessMutex, INFINITE);
        if ( waitResult != WAIT_OBJECT_0 )
            return false;
        if ( rwlock->rw_refCount == 0 )    //读写锁还没有被锁定
        {
            waitResult = WaitForSingleObject(rwlock->rw_stateChangeMutex.stateMutex, INFINITE);
            if ( waitResult == WAIT_OBJECT_0 )
            {
                rwlock->rw_refCount = -1;
                ResetEvent(rwlock->rw_unlockEvent);
                ReleaseMutex(rwlock->rw_stateChangeMutex.stateMutex);
            }
            else
                bResult = false;
        }
        else    //读写锁已被锁定，可能是读锁，也可能是写锁
        {
            waitResult = WaitForSingleObject(rwlock->rw_unlockEvent, INFINITE);
            if ( waitResult == WAIT_OBJECT_0 )
            {
                waitResult = WaitForSingleObject(rwlock->rw_stateChangeMutex.stateMutex, INFINITE);
                if ( waitResult == WAIT_OBJECT_0 )
                {
                    rwlock->rw_refCount = -1;
                    ResetEvent(rwlock->rw_unlockEvent);
                    ReleaseMutex(rwlock->rw_stateChangeMutex.stateMutex);
                }
                else
                    bResult = false;
            }
            else
                bResult = false;
        }
        ReleaseMutex(rwlock->rw_accessMutex.accessMutex);
    }
    else
    {
        EnterCriticalSection(&rwlock->rw_accessMutex.csAccess);
        if ( rwlock->rw_refCount == 0 )     //读写锁还没有被锁定
        {
            EnterCriticalSection(&rwlock->rw_stateChangeMutex.csState);
            rwlock->rw_refCount = -1;
            ResetEvent(rwlock->rw_unlockEvent);
            LeaveCriticalSection(&rwlock->rw_stateChangeMutex.csState);
        }
        else    //已经被写锁定，则等待写锁被释放
        {
            waitResult = WaitForSingleObject(rwlock->rw_unlockEvent, INFINITE);
            if ( waitResult == WAIT_OBJECT_0 )
            {
                EnterCriticalSection(&rwlock->rw_stateChangeMutex.csState);
                rwlock->rw_refCount = -1;
                ResetEvent(rwlock->rw_unlockEvent);
                LeaveCriticalSection(&rwlock->rw_stateChangeMutex.csState);
            }
            else
                bResult = false;
        }
        LeaveCriticalSection(&rwlock->rw_accessMutex.csAccess);
    }
    return bResult;
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    int ret = pthread_rwlock_wrlock(rwlock);
    if ( ret != 0 )
        return false;
#else
    int ret = rw_wrlock(rwlock);
    if ( ret != 0 )
        return false;
#endif
    return true;
}

bool 
RSMutex::TryWrLock()
{
    if(m_rwlock == NULL)
        return false;

    RwLock_t * rwlock = (RwLock_t*)m_rwlock;
#if defined (WIN32) || defined (_WINDOWS)
    DWORD waitResult;
    bool bResult = true;
    if ( m_bpshared == true )
    {
        //使用读写锁的权限
        waitResult = WaitForSingleObject(rwlock->rw_accessMutex.accessMutex, 0);
        if ( waitResult != WAIT_OBJECT_0 )
            return false;
        if ( rwlock->rw_refCount == 0 )     //读写锁还没有被锁定
        {
            //改变rw_refcount状态的权限
            waitResult = WaitForSingleObject(rwlock->rw_stateChangeMutex.stateMutex, 0);
            if ( waitResult == WAIT_OBJECT_0 )
            {
                rwlock->rw_refCount = -1;
                ResetEvent(rwlock->rw_unlockEvent); //设置为nosignal状态，让其它进程(或线程)等
                ReleaseMutex(rwlock->rw_stateChangeMutex.stateMutex);
            }
            else
                bResult = false;
        }
        else    //已经被写锁定，则等待写锁被释放
        {
            waitResult = WaitForSingleObject(rwlock->rw_unlockEvent, 0);
            if ( waitResult == WAIT_OBJECT_0 )      //写锁已释放
            {
                waitResult = WaitForSingleObject(rwlock->rw_stateChangeMutex.stateMutex, 0);
                if ( waitResult == WAIT_OBJECT_0 )
                {
                    rwlock->rw_refCount = -1;
                    ResetEvent(rwlock->rw_unlockEvent);
                    ReleaseMutex(rwlock->rw_stateChangeMutex.stateMutex);
                }
                else 
                    bResult = false;
            }
            else
                bResult = false;
        }
        ReleaseMutex(rwlock->rw_accessMutex.accessMutex);
    }
    else
    {
        if ( TryEnterCriticalSection(&rwlock->rw_accessMutex.csAccess) == FALSE )
            return false;
        if ( rwlock->rw_refCount == 0 )     //读写锁还没有被锁定
        {
            if ( TryEnterCriticalSection(&rwlock->rw_stateChangeMutex.csState) == TRUE )
            {
                rwlock->rw_refCount = -1;
                ResetEvent(rwlock->rw_unlockEvent);
                LeaveCriticalSection(&rwlock->rw_stateChangeMutex.csState);
            }
            else
                bResult = false;
        }
        else    //已经被写锁定，则等待写锁被释放
        {
            waitResult = WaitForSingleObject(rwlock->rw_unlockEvent, 0);
            if ( waitResult == WAIT_OBJECT_0 )
            {
                if ( TryEnterCriticalSection(&rwlock->rw_stateChangeMutex.csState) == TRUE )
                {
                    rwlock->rw_refCount = -1;
                    ResetEvent(rwlock->rw_unlockEvent);
                    LeaveCriticalSection(&rwlock->rw_stateChangeMutex.csState);
                }
                else
                    bResult = false;
            }
            else
                bResult = false;
        }
        LeaveCriticalSection(&rwlock->rw_accessMutex.csAccess);
    }
    return bResult;
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    int ret = pthread_rwlock_trywrlock(rwlock);
    if ( ret != 0 )
        return false;
#else
    int ret = rw_trywrlock(rwlock);
    if ( ret != 0 )
        return false;
#endif
    return true;
}

bool 
RSMutex::Unlock()
{
    if(m_rwlock == NULL)
        return false;

    RwLock_t* rwlock = (RwLock_t*)m_rwlock;
#if defined (WIN32) || defined (_WINDOWS)
    DWORD waitResult;
    bool bResult = true;
    if ( m_bpshared == true )
    {
        waitResult = WaitForSingleObject(rwlock->rw_stateChangeMutex.stateMutex, INFINITE);
        if ( waitResult == WAIT_OBJECT_0 )
        {
            if ( rwlock->rw_refCount == -1 )    //已加读锁
            {
                rwlock->rw_refCount = 0;
                SetEvent(rwlock->rw_unlockEvent);
            }
            else if ( rwlock->rw_refCount > 0 ) //没有锁定
            {
                rwlock->rw_refCount--;
                SetEvent(rwlock->rw_unlockEvent);
            }
        }
        else
            bResult = FALSE;
    }
    else
    {
        EnterCriticalSection(&rwlock->rw_stateChangeMutex.csState);
        if ( rwlock->rw_refCount == -1 )    //已加读锁
        {
            rwlock->rw_refCount = 0;
            SetEvent(rwlock->rw_unlockEvent);
        }
        else if ( rwlock->rw_refCount > 0 )
        {
            rwlock->rw_refCount--;
            SetEvent(rwlock->rw_unlockEvent);
        }
        LeaveCriticalSection(&rwlock->rw_stateChangeMutex.csState);
    }
    return bResult;
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    int ret = pthread_rwlock_unlock(rwlock);
    if ( ret != 0 )
        return false;
#else
    int ret = rw_unlock(rwlock);
    if ( ret != 0 )
        return false;
#endif
    return true;
}
