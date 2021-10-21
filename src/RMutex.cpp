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

#include <RMutex.hpp>

//namespace athena
//{
#if defined (WIN32) || defined (_WINDOWS)
    #include <windows.h>
    #include <limits.h>
    typedef struct
    {
        HANDLE h;
    }
    Mutex_t;
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H) || \
	  defined(HAVE_PTHREAD_WIN32)
    #include <pthread.h>
    typedef pthread_mutex_t Mutex_t;
#elif defined(HAVE_THREAD_H)   //solaris
    #include <thread.h>
    typedef mutex_t Mutex_t;
#endif

//}

using namespace athena;

RMutex::RMutex(bool process /*= false*/)
              :m_mut(NULL)
{
    Mutex_t *mut = NULL;
    mut = new Mutex_t;
#if defined (WIN32) || defined (_WINDOWS)
    if((mut->h = CreateMutex(NULL, FALSE, NULL)) == NULL)
    {
        delete mut;
        mut = NULL;
    }
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
	pthread_mutexattr_t mutexattr;
	pthread_mutexattr_init( &mutexattr );
	#ifdef PTHREAD_MUTEX_RECURSIVE
	    pthread_mutexattr_settype( &mutexattr, PTHREAD_MUTEX_RECURSIVE );
	#endif
	if ( process == true )  //·ñÔòÄ¬ÈÏÎª PTHREAD_PROCESS_PRIVATE
	    pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED);
    int i = pthread_mutex_init( mut, &mutexattr );
    pthread_mutexattr_destroy( &mutexattr );
    if ( i != 0 )
    {
        delete mut;
        mut = NULL;
    }
#elif defined(HAVE_THREAD_H)   //solaris
    int type = 0;
    if ( process == true )
        type |= USYNC_PROCESS_ROBUST;
    int i = mutex_init(mut, type, NULL);
    if ( i != 0 )
    {
        delete mut;
        mut = NULL;
    }
#endif
    m_mut = (struct RMutex_t *)mut;
}

RMutex::~RMutex()
{
    if(m_mut != NULL)
    {
        Mutex_t * mut = (Mutex_t*)m_mut;
#if defined (WIN32) || defined (_WINDOWS)
        CloseHandle(mut->h);
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
        pthread_mutex_destroy(mut);
#elif defined(HAVE_THREAD_H)   //solaris
        mutex_destroy(mut);
#endif
        delete mut;
        mut = NULL;
        m_mut = NULL;
    }
}

bool 
RMutex::Lock()
{
    if(m_mut == NULL)
        return false;

    Mutex_t * mut = (Mutex_t*)m_mut;
#if defined (WIN32) || defined (_WINDOWS)
    DWORD err;
    if((err = WaitForSingleObject (mut->h, INFINITE)) != WAIT_OBJECT_0)
    {
        return false;
    }
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    int ret = pthread_mutex_lock(mut);
    if ( ret != 0 )
        return false;
#else
    int ret = mutex_lock(mut);
    if ( ret != 0 )
        return false;
#endif
    return true;
}

bool 
RMutex::TryLock()
{
    if(m_mut == NULL)
        return false;

    Mutex_t * mut = (Mutex_t*)m_mut;
#if defined (WIN32) || defined (_WINDOWS)
    DWORD err;
    if((err = WaitForSingleObject (mut->h, 0)) != WAIT_OBJECT_0)
    {
        return false;
    }
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    int ret = pthread_mutex_trylock(mut);
    if ( ret != 0 )
        return false;
#else
    int ret = mutex_trylock(mut);
    if ( ret != 0 )
        return false;
#endif
    return true;
}

bool 
RMutex::Unlock()
{
    if(m_mut == NULL)
        return false;

    Mutex_t * mut = (Mutex_t*)m_mut;
#if defined (WIN32) || defined (_WINDOWS)
    ReleaseMutex(mut->h);
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H)
    int ret = pthread_mutex_unlock(mut);
    if ( ret != 0 )
        return false;
#else
    int ret = mutex_unlock(mut);
    if ( ret != 0 )
        return false;
#endif
    return true;
}
