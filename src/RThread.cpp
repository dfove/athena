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

#include <algorithm>
#include <functional>
#include <RThread.hpp>
#include <RTime.hpp>
#include <signal.h>
#include <assert.h>
#include <string.h>

namespace athena
{
#if defined(WIN32) || defined(_WINDOWS)
    #include <windows.h>
    #include <limits.h>
    #include <process.h>
    typedef struct
    {
        HANDLE h;
    }
    Thread_t;

#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H) || \
	  defined(HAVE_PTHREAD_WIN32)
    #include <unistd.h>
    #include <pthread.h>
    typedef pthread_t Thread_t;
#elif defined(HAVE_THREAD_H)   //solaris
    #include <unistd.h>
    #include <thread.h>
    typedef thread_t Thread_t;
#endif

}

using namespace athena;

RThread::RThread(const char* lpszname /*= NULL*/, bool bAutoDelete /*= true*/, bool bCanReload /*= true*/)
                :m_thr(NULL), 
                 m_parentID(0), 
                 m_name(lpszname?lpszname:""),
                 m_bAutoDelete(bAutoDelete), 
                 m_bRun(false),
                 m_bCanReload(bCanReload),
                 m_bReload(false),
                 m_loopTimes(-1),
                 m_detached(false),
                 m_defaultExecution(true),
                 m_userDefinedExecution(NULL),
                 m_userDefinedLoopExecution(NULL),
                 m_threadArg(NULL)
{
}

RThread::~RThread()
{
/*
 *  不能在析构函数中调用Exit()和Join()函数
 *  因为：1、在销毁线程类的时候，处理线程在逻辑上可能还没有完成必要的任务;
 *        2、在销毁线程类的时候，如果处理线程处理时间很长，主线程调用Join()会阻塞知道处理线程退出
 *        3、因为处理线程分配资源需要一点时间，有可能在销毁线程类的时候，处理线程还没启动就退出了
 */
//    if ( m_bRun != 0 )
//    {
//        Exit();
//        Release();
//    }
}

inline bool 
RThread::BeforeExec()
{
    return true;
}

inline bool 
RThread::BeforeReload()
{
    return true;
}

inline void*  
RThread::Exec()
{
    return NULL;
}

inline bool 
RThread::AfterExec()
{
    return true;
}

bool 
RThread::Create(long looptimes /* = -1 */, 
                bool detached /*= false*/, 
                bool boundlwp /*= false*/)
{
    if ( m_bRun != false )
        return false;
    /*
     *  首先将运行状态置为1，如果启动线程失败则重置为0
     *  不能再调用创建线程函数之后设置为正在运行状态，因为:
     *      可能子线程在父线程设置m_bRun之前就开始运行Execution了，
     *      此时m_bRun可能还没被设置为1，从而子线程提前退出
     */
    m_parentID = GetCurrentThreadId();
    m_bRun = true;
    Thread_t *thr = new Thread_t;
    m_thr = (struct RThread_t*)thr;
    memset(thr, 0, sizeof(Thread_t));
    m_loopTimes = looptimes;
#if defined(WIN32) || defined(_WINDOWS)
    thr->h = (HANDLE)_beginthreadex(NULL, 0, (unsigned ( __stdcall *)(void*))Execution, (void*)this, 0, NULL);
    if ( thr->h == 0 )
    {
        m_parentID = 0;
        m_bRun = false;
        delete thr;
        thr = NULL;
        return false;
    }
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H)
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    m_detached = detached;
    if ( m_detached == true )
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if ( boundlwp )
        pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    int ret = pthread_create(thr, &attr, Execution, (void*)this);
    pthread_attr_destroy(&attr);
    if ( ret != 0)
    {
        m_parentID = 0;
        m_bRun = false;
        delete thr;
        thr = NULL;
        return false;
    }
#elif defined(HAVE_THREAD_H)   //solaris
    long flags = 0;
    m_detached = detached;
    if ( m_detached == true )
    {
        flags &= THR_DETACHED;
    }
    if ( boundlwp )
        flags &= THR_BOUND;
    int ret = thr_create(NULL, 0, Execution, (void*)this, 0, thr);
    if ( ret != 0 )
    {
        m_parentID = 0;
        m_bRun = false;
        delete thr;
        thr = NULL;
        return false;
    }
#endif
    return true;
}

bool 
RThread::Create(int thrType)
{
    bool detach = false;
    bool boundLwp = false;
    if ( thrType & RThread::Thr_Detach )
        detach = true;
    if ( thrType & RThread::Thr_Globe )
        boundLwp = true;
    return Create(1, detach, boundLwp);
}

bool 
RThread::Create(void*(*start_routine)(void *), void* arg, long looptimes /*= -1*/,
                bool detached /*= false*/, bool boundlwp /*= false*/)
{
    if ( m_bRun != false )
        return false;
    m_defaultExecution = false;
    m_userDefinedExecution = start_routine;
    m_userDefinedLoopExecution = NULL;
    m_threadArg = arg;
    return Create(looptimes, detached, boundlwp);
}

bool 
RThread::Create(void*(*start_routine_selfloop)(RThread &, void *), void* arg, long looptimes /*= -1*/,
                bool detached /*= false*/, bool boundlwp /*= false*/)
{
    if ( m_bRun != false )
        return false;
    m_defaultExecution = false;
    m_userDefinedExecution = NULL;
    m_userDefinedLoopExecution = start_routine_selfloop;
    m_threadArg = arg;
    return Create(looptimes, detached, boundlwp);
}

void  
RThread::SetCancelPoint(void)
{
    if( m_bRun == false )       //设置为退出
    {
        throw 1;
    }
    else if( m_bReload == true)   //设置为重载
    {
        if( m_bCanReload )
        {
            throw 2;
        }
        else
            m_bReload = false;
    }
}

bool 
RThread::Reload()
{
    if ( m_bCanReload == false )
        return false;
    m_bReload = true;
    return true;
}

bool 
RThread::Exit()
{
    m_bRun = false;
#if defined(WIN32) || defined(_WINDOWS)
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H)
    pthread_kill(GetThreadID(), SIGINT);
#elif defined(HAVE_THREAD_H)   //solaris
    thr_kill(GetThreadID(), SIGINT);
#endif
    return true;
}

bool 
RThread::Release()
{
    if ( m_thr == NULL )
        return true;
    Thread_t *thr = (Thread_t *)m_thr;
#if defined(WIN32) || defined(_WINDOWS)
    int i = WaitForSingleObject (thr->h, INFINITE);
    if (i != WAIT_OBJECT_0)
    {
        CloseHandle (thr->h);
        delete thr;
        thr = NULL;
        m_thr = NULL;
        return false;
    }
    CloseHandle (thr->h);
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H)
    if ( m_detached == true )
        return true;
    int ret = pthread_join(*thr, NULL);
    if ( ret != 0 )
    {
        delete thr;
        m_thr = NULL;
        return false;
    }
#elif defined(HAVE_THREAD_H)   //solaris
    if ( m_detached == true )
        return true;
    int ret = thr_join(*thr, NULL, NULL);
    if ( ret != 0 )
    {
        delete thr;
        m_thr = NULL;
        return false;
    }
#endif
    delete thr;
    thr = NULL;
    m_thr = NULL;
    return true;
}

unsigned long
RThread::GetThreadID() const
{
     if ( m_thr == NULL )
        return 0;
     Thread_t *thr = (Thread_t *)m_thr;
#if defined(WIN32) || defined(_WINDOWS)
    return (unsigned long)thr->h;
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H)
    return (unsigned long)(*thr);
#elif defined(HAVE_THREAD_H)   //solaris
    return (unsigned long)(*thr);
#endif
}

const RString& 
RThread::GetName() const
{
    return m_name;
}

unsigned long 
RThread::GetParentID() const
{
    return m_parentID;
}

inline bool 
RThread::IsParent(unsigned long parentID) const
{
    return m_parentID == parentID;
}

inline bool 
RThread::IsDetached() const
{
    return m_detached;
}

inline bool 
RThread::IsAutoDelete(void) const
{
    return m_bAutoDelete;
}

unsigned long 
RThread::GetCurrentThreadId()
{
#if defined(WIN32) || defined(_WINDOWS)
    return ::GetCurrentThreadId();
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H)
    return pthread_self();
#elif defined(HAVE_THREAD_H)   //solaris
    return thread_self();
#endif
}

size_t
RThread::GetThreadCount(void)
{
    return RThreadManager::Instance().GetCount();
}

void
RThread::SetAllExit(void) 
{
    RThreadManager::Instance().SetAllExit();
}

void
RThread::SetAllReload(void)
{
    RThreadManager::Instance().SetAllReload();
}

RThread* 
RThread::GetSelfPThread(void)
{
    unsigned long dwID = RThread::GetCurrentThreadId();
    return RThreadManager::Instance().GetFromID(dwID);
}

RThread* 
RThread::GetThreadByName(const RString& thrName)
{
    return RThreadManager::Instance().GetFromName(thrName);
}

size_t 
RThread::GetChildCount(void)
{
    return RThreadManager::Instance().GetChildCount();
}

bool 
RThread::Release(unsigned long dwID)
{
#if defined(WIN32) || defined(_WINDOWS)
    HANDLE h = (HANDLE)dwID;
    int i = WaitForSingleObject (h, INFINITE);
    if (i != WAIT_OBJECT_0)
    {
        CloseHandle (h);
        return false;
    }
    CloseHandle (h);
#elif defined(HAVE_PTHREAD) || defined(HAVE_PTHREAD_H) || defined(HAVE_PTH_PTHREAD_H)
    pthread_t thr = (pthread_t)dwID;
    int ret = pthread_join(thr, NULL);
    if ( ret != 0 )
    {
        return false;
    }
#elif defined(HAVE_THREAD_H)   //solaris
    thread_t thr = (thread_t)dwID;
    int ret = thr_join(thr, NULL, NULL);
    if ( ret != 0 )
    {
        return false;
    }
#endif
    return true;
}

void 
RThread::Terminate(RThread* pThread)
{
    if ( NULL == pThread )
        return;
    unsigned long dwID = 0;
    if( !pThread->IsDetached() )
    {
        dwID = pThread->GetThreadID();
    }
    pThread->Exit();
    if(dwID)
    {
        RThread::Release(dwID);
    }
}

void* 
RThread::Execution(void* arg)
{
    RThread* self = (RThread*)arg;
    RASSERT(self);
    RThreadManager::Instance().AddThread(self);
    self->BeforeExec();
    void* ret = NULL;
    if ( self->m_loopTimes == -1 )
    {
        while( self->m_bRun )          //线程无限循环，直到显示调用Exit()
        {
            try
            {
                if ( self->m_bReload == true )    //重载
                {
                    if ( self->m_bCanReload )
                        self->BeforeReload();
                    self->m_bReload = false;
                }
                if ( self->m_defaultExecution == true )
                {
                    ret = self->Exec();
                }
                else
                {
                    if ( self->m_userDefinedExecution != NULL )
                        ret = self->m_userDefinedExecution(self->m_threadArg);
                    else if ( self->m_userDefinedLoopExecution != NULL )
                        ret = self->m_userDefinedLoopExecution(*self, self->m_threadArg);
                }
            }
            catch (int e)
            {
                if(e == 1)      //退出
                {
                    break;
                }
                else if(e == 2) //重载
                {
                    self->BeforeReload();
                    self->m_bReload = false;
                }
            }
        }
    }
    else
    {
        while(self->m_bRun && self->m_loopTimes--)
        {
            try
            {
                if ( self->m_bReload )    //重载
                {
                    if ( self->m_bCanReload )
                        self->BeforeReload();
                    self->m_bReload = false;
                }
                if ( self->m_defaultExecution == true )
                {
                    ret = self->Exec();
                }
                else
                {
                    if ( self->m_userDefinedExecution != NULL )
                    {
                        ret = self->m_userDefinedExecution(self->m_threadArg);
                    }
                    else if ( self->m_userDefinedLoopExecution != NULL )
                        ret = self->m_userDefinedLoopExecution(*self, self->m_threadArg);
                }
            }
            catch ( int e )
            {
                if(e == 1)      //退出
                {
                    break;
                }
                else if(e == 2) //重载
                {
                    self->m_loopTimes++;    //如果是捕获循环中的异常，则补回一次循环次数(对m_loopTimes==1，但是是无限循环特别有用)
                    self->BeforeReload();
                    self->m_bReload = false;
                }
            }
        }
    }
    self->m_bRun = false;
    self->AfterExec();
    RThreadManager::Instance().ExitThread(self);
    return ret;
}


/*************************************************************************************
* RThreadManager    -　线程管理类
* 说明：　　提供对所有从RThread派生类的管理
* 使用了以线程ID为索引的递增数组
**************************************************************************************/

class Less_PThread: public std::binary_function<const RThread*, const RThread*, bool>
{//辅助用于定位的函数对象，也可以使用函数 bool Less_PThread(const RThread*, const RThread*);
public:
    bool operator()(const RThread* _Pleft, const RThread* _Right) const
    {// call function with operand
        RASSERT(_Pleft);
        return _Pleft->GetThreadID() < _Right->GetThreadID();
    }
};

RThreadManager::RThreadManager(void)
{

}

RThreadManager& 
RThreadManager::Instance(void)
{
    static RThreadManager _instance;
    return _instance;
}

void 
RThreadManager::AddThread(RThread* pThread)
{
    WRLOCK(m_Mutex);
    //使插入时便有序
    /*PThread_Iterator iter = */m_Data.insert(lower_bound(m_Data.begin(),m_Data.end(),pThread,Less_PThread()),pThread);
}

void
RThreadManager::ExitThread(RThread* pThread)
{
    unsigned long dwID = pThread->GetThreadID();
    {
        WRLOCK(m_Mutex);
        PThread_Iterator p = lower_bound(m_Data.begin(),m_Data.end(),pThread,Less_PThread());
        if(p != m_Data.end() && (*p)->GetThreadID() == dwID)
        {
            m_Data.erase(p);
        }
    }

    if(pThread && pThread->IsAutoDelete())
    {
        delete pThread;
    }
}

size_t
RThreadManager::GetCount(void)
{
    RDLOCK(m_Mutex);
    return m_Data.size();
}

void
RThreadManager::SetAllExit(void)
{
    std::vector<unsigned long> attach_vdwID;        //取出非分离的线程ID
    {
        RDLOCK(m_Mutex);
        for(R_PThread_Iterator p = m_Data.rbegin();p != m_Data.rend();p++)
        {
            if( (*p)->IsDetached() == false )
            {//对于非分离的线程等待期退出
                attach_vdwID.push_back((*p)->GetThreadID());
            }
        }
    }

    {
        WRLOCK(m_Mutex);
        for(R_PThread_Iterator p = m_Data.rbegin();p != m_Data.rend();p++)
        {
            (*p)->Exit();
        }
    }

    std::vector<unsigned long>::iterator iIter;
    for ( iIter = attach_vdwID.begin(); iIter != attach_vdwID.end(); iIter++)
        RThread::Release(*iIter);

//    while( GetCount() != 0 )
//    {//对于分离线程要等待其退出，并从RThreadManager对象中删除
//        RTime::Sleep_r(1000);
//    }
}

void
RThreadManager::SetAllReload(void)
{
    WRLOCK(m_Mutex);
    for(R_PThread_Iterator p = m_Data.rbegin();p != m_Data.rend();p++)
    {
        (*p)->Reload();
    }
}

RThread*
RThreadManager::GetFromID(unsigned long dwID)
{
    RDLOCK(m_Mutex);
//    PThread_Iterator p = lower_bound(m_Data.begin(),m_Data.end(),dwID,Less_PThread());
    PThread_Iterator iter;
    for (iter = m_Data.begin(); iter != m_Data.end(); iter++)
    {
        if ( (*iter)->GetThreadID() >= dwID )
            break;
    }
    if(iter != m_Data.end() && (*iter)->GetThreadID() == dwID)
    {
        return *iter;
    }
    return NULL;
}

RThread* 
RThreadManager::GetFromName(const RString& thrName)
{
    RDLOCK(m_Mutex);
    for(PThread_Iterator p = m_Data.begin(); p != m_Data.end(); p++)
	{
		if( (*p)->GetName() == thrName )
            return *p;
	}
	return NULL;
}

RThread*
RThreadManager::GetParent(void)
{
    unsigned long currentID = RThread::GetCurrentThreadId();
    RThread* currentThread = GetFromID(currentID);
    return GetFromID(currentThread->GetParentID());
    return NULL;
}

size_t
RThreadManager::GetChildCount(void)
{
    unsigned long dwID = RThread::GetCurrentThreadId();
    RDLOCK(m_Mutex);
	size_t c=0;
    for(PThread_Iterator p = m_Data.begin(); p != m_Data.end(); p++)
	{
		if((*p)->IsParent(dwID))
			++c;
	}
	return c;
}

bool
RThreadManager::CheckExist(RThread* pThread)
{
    if(NULL == pThread)
        return false;

    for(PThread_Iterator p = m_Data.begin();p != m_Data.end();++p)
    {
        if(*p == pThread && (*p)->GetThreadID() == pThread->GetThreadID())
        {
            return true;
        }
    }
    return false;
}

bool 
RThreadManager::CheckExist(unsigned long dwID)
{
    RDLOCK(m_Mutex);
    PThread_Iterator iter;
    for (iter = m_Data.begin(); iter != m_Data.end(); iter++)
    {
        if ( (*iter)->GetThreadID() >= dwID )
            break;
    }
    if(iter != m_Data.end() && (*iter)->GetThreadID() == dwID)
    {
        return true;
    }
    return false;
}
