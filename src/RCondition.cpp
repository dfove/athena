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

#include <RCondition.hpp>
#include <RMutex.hpp>

namespace athena
{
#if defined (WIN32) || defined (_WINDOWS)
    #include <windows.h>
    typedef struct 
    {
        HANDLE h;
    }
    Cond_t;

#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H) || \
	  defined(HAVE_PTHREAD_WIN32)
    #include <sys/time.h>
    #include <pthread.h>
    #include <errno.h>
    typedef pthread_cond_t Cond_t;

#elif defined(HAVE_THREAD_H)
    #include <sys/time.h>
    #include <thread.h>
    #include <errno.h>
    typedef cond_t Cond_t;
#endif
}

using namespace athena;

RCondition::RCondition(bool process /*= false*/)
                      :m_cond(NULL), 
                       m_mutex(NULL)
{
    Cond_t *cond = new Cond_t;
#if defined (WIN32) || defined (_WINDOWS)
    cond->h = CreateEvent( 
        NULL,         // no security attributes
        false,         // auto-reset event
        false,         // initial state is not signaled
        NULL  // object no name
        );
    if ( cond->h == NULL )
    {
        delete cond;
        cond = NULL;
    }
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    pthread_condattr_t condattr;
    pthread_condattr_init(&condattr);
    if ( process == true )
        pthread_condattr_setpshared(&condattr, PTHREAD_PROCESS_SHARED); //Ä¬ÈÏÎª PTHREAD_PROCESS_PRIVATE
    int i = pthread_cond_init(cond, NULL);
    pthread_condattr_destroy(&condattr);
    if (i != 0)
    {
        delete cond;
        cond = NULL;
    }
#elif defined(HAVE_THREAD_H)
    int type = 0;
    if ( process == true )
        type |= USYNC_PROCESS;
    int i = cond_init(cond, type, NULL);
    if ( i != 0 )
    {
        delete cond;
        cond = NULL;
    }
#endif
    m_cond = (RCond_t *)cond;
    if ( m_cond != NULL )
        m_mutex = new RMutex(process);
}

RCondition::~RCondition()
{
    if( m_cond != NULL )
    {
        Cond_t *cond = (Cond_t *)m_cond;
#if defined (WIN32) || defined (_WINDOWS)
        CloseHandle(cond->h);
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
        pthread_cond_destroy(cond);
#elif defined(HAVE_THREAD_H)
        cond_destroy(cond);
#endif
        delete cond;
        cond = NULL;
        m_cond = NULL;
        delete m_mutex;
        m_mutex = NULL;
    }
}

bool 
RCondition::CondLock()
{
    if ( m_mutex == NULL )
        return false;
    return m_mutex->Lock();
}

bool 
RCondition::CondUnlock()
{
    if ( m_mutex == false )
        return false;
    return m_mutex->Unlock();
}

bool 
RCondition::Wait()
{
    if ( m_cond == NULL || m_mutex == NULL )
        return false;
    Cond_t *cond = (Cond_t *)m_cond;
#if defined (WIN32) || defined (_WINDOWS)
    DWORD err;
    if((err = WaitForSingleObject (cond->h, INFINITE)) != WAIT_OBJECT_0)
    {
        return false;
    }
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    struct RMutex_t * gmut = m_mutex->m_mut;
    pthread_mutex_t * pmut = (pthread_mutex_t*)gmut;
    if ( pmut == NULL )
        return false;
    int ret = pthread_cond_wait(cond, pmut);
    if (ret != 0)
        return false;
#elif defined(HAVE_THREAD_H)
    struct RMutex_t * gmut = m_mutex->m_mut;
    mutex_t * pmut = (mutex_t*)gmut;
    if ( pmut == NULL )
        return false;
    int ret = cond_wait(cond, pmut);
    if (ret != 0)
        return false;
#endif
    return true;
}

int 
RCondition::TimedWait(unsigned int milliseconds)
{
    if ( m_cond == NULL || m_mutex == NULL )
        return -1;
    Cond_t* cond = (Cond_t*)m_cond;
#if defined (WIN32) || defined (_WINDOWS)
    DWORD err = WaitForSingleObject (cond->h, milliseconds);
    if( err == WAIT_OBJECT_0)
    {
        return 0;
    }
    else if ( err == WAIT_TIMEOUT )
    {
        return 1;
    }
    else
        return err;
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    struct RMutex_t * gmut = m_mutex->m_mut;
    pthread_mutex_t * pmut = (pthread_mutex_t*)gmut;
    if ( pmut == NULL )
        return -1;
    unsigned int nMs = milliseconds==0?1:milliseconds;
    /*
     *  struct timespec {
     *        time_t      tv_sec;     // seconds
     *        suseconds_t tv_nsec;    // microseconds ÄÉÃë
     *    };
     */
    struct timespec ts;
    /*
     *  struct timeval {
     *        time_t      tv_sec;     // seconds
     *        suseconds_t tv_usec;    // microseconds Î¢Ãë
     *    };
     */
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + tv.tv_usec/10000000 + nMs/1000;
    ts.tv_nsec = ( tv.tv_usec % 1000000 ) * 1000 + ( nMs % 1000 ) * 1000000;
    int ret = pthread_cond_timedwait(cond, pmut, &ts);
    if ( ret == ETIMEDOUT )
        return 1;
    return ret;
#elif defined(HAVE_THREAD_H)
    struct RMutex_t * gmut = m_mutex->m_mut;
    mutex_t * pmut = (mutex_t*)gmut;
    if ( pmut == NULL )
        return -1;
    unsigned int nMs = milliseconds==0?1:milliseconds;
    struct timespec ts;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + tv.tv_usec/10000000 + nMs/1000;
    ts.tv_nsec = ( tv.tv_usec % 1000000 ) * 1000 + ( nMs % 1000 ) * 1000000;
    int ret = cond_timedwait(cond, pmut, &ts);
    if ( ret == ETIME )
        return 1;
    return ret;
#endif
}

bool 
RCondition::Signal()
{
    if ( m_cond == NULL || m_mutex == NULL )
        return false;
    Cond_t *cond = (Cond_t *)m_cond;
#if defined (WIN32) || defined (_WINDOWS)
    int ret = SetEvent(cond->h);
    if ( ret == false )
    {
        return false;
    }
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    int ret = pthread_cond_signal(cond);
    if ( ret != 0 )
    {
        return false;
    }
#elif defined(HAVE_THREAD_H)
    int ret = cond_signal(cond);
    if ( ret != 0 )
    {
        return false;
    }
#endif
    return true;
}

bool 
RCondition::Broadcast()
{
    if ( m_cond == NULL || m_mutex == NULL )
        return false;
    Cond_t *cond = (Cond_t *)m_cond;
#if defined (WIN32) || defined (_WINDOWS)
    int ret = SetEvent(cond->h);
    if ( ret == false )
    {
        return false;
    }
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    int ret = pthread_cond_broadcast(cond);
    if (ret != 0)
    {
        return false;
    }
#elif defined(HAVE_THREAD_H)
    int ret = cond_broadcast(cond);
    if (ret != 0)
    {
        return false;
    }
#endif
    return true;
}
