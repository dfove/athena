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

#include <iostream>
#include "RThread.hpp"
#include <RMutex.hpp>
#include <RTime.hpp>
#include <RTools.hpp>
#if defined(_WIN32) || defined(_WINDOWS)
  #include <windows.h>
  #define SLEEP(val) Sleep(val*1000)
#else
  #include <unistd.h>
  #include <signal.h>
  #include <stdio.h>
  #define SLEEP(val) sleep(val)
#endif

using namespace std;
using namespace athena;

#define THREADS_NUM 10

void* func(void* arg)
{
    int* i = (int*)arg;
    //cout <<"--------------------" << *i << endl;
    //free(arg);
    (*i)++;
    return NULL;
}

void loopfuncSignal(int sig)
{
    printf("thread %ld recv signal %d!\n", pthread_self(), sig);
    pthread_exit(NULL);
    return;
}

void* loopfunc(RThread& thread, void * arg)
{
    signal(SIGUSR1, loopfuncSignal);
    while(true)
    {
        thread.SetCancelPoint();
        int* i = (int*)arg;
        //cout <<"--------------------" << *i << endl;
        //free(arg);
        (*i)++;
        RTime::Sleep_r(1);
    }
    return NULL;
}

class NewThread : public RThread
{
public:
    NewThread(const char* lpszname="newthread", 
              bool bAutoDelete = true, 
              bool canreload=true):RThread(lpszname, bAutoDelete, canreload)
    {
        m_threadArg = new(int);
    }

    virtual ~NewThread()
    {
//        int* parg = (int*)m_threadArg;
//        delete parg;
//        m_threadArg = NULL;
    }
public:
    void SetArg(int i)
    {
        int* parg = (int*)m_threadArg;
        *parg = i;
    }

    int GetArg()
    {
        int* parg = (int*)m_threadArg;
        return *parg;
    }

    virtual void* Exec()
    {
        int* i = (int*)m_threadArg;
        //cout <<"--------------------" << *i << endl;
        //free(arg);
        (*i)++;
        return NULL;
    }

    virtual bool BeforeReload()
    {
        cout << "newthread " << endl;
        int* i = (int*)m_threadArg;
        (*i) *= 10;
        return true;
    }
};

static RMutex mut;
static int flag = 0;
void* mutexfunc(void* arg)
{
    mut.Lock();
    flag++;
//    TimeFunc::Sleep_r(1);
    printf("flag is %d\n", flag);
    int* i = (int*)arg;
    //cout <<"--------------------" << *i << endl;
    //free(arg);
    (*i)++;
    mut.Unlock();
    return NULL;
}

int main()
{
    int success = 0, failed = 0;
    bool ret;
    RThread* threads[THREADS_NUM];
    //NewThread* threads[THREADS_NUM];
    //int  oarg[THREADS_NUM];
    int* arg[THREADS_NUM];
    for(int i=0; i < THREADS_NUM; ++i) 
    {
        arg[i] = new(int);
        //arg[i] = &oarg[i];
        //*arg[i] = i;
        *arg[i] = 0;
        threads[i] = new NewThread("newthread");
        //threads[i]->SetArg(0);
        //threads[i] = new RThread("oldthread");
        //ret = threads[i]->Create(func, (void*)arg[i], 10);
        ret = threads[i]->Create(loopfunc, (void*)arg[i]);
        //ret = threads[i]->Create(10);
        //ret = threads[i]->Create(mutexfunc, (void*)arg[i], 10, true);
        if ( ret == true )
            success++;
        else
            failed++;
    }
    cout<< "success :" << success << " failed :" << failed << endl;
//    for (int i = 0; i < THREADS_NUM; i++)
//    {
//        threads[i]->Reload();
//    }
//    SLEEP(5);

    RString sFilePath(GetExeFilePath());//得到程序的全路径名
    RString m_sExecDir = GetParentDirPath(sFilePath);//得到程序目录
    RString m_sExecName = GetNameFromFullPath(sFilePath);//得到程序名
    cout << "m_sExecDir:" << m_sExecDir << endl;
    cout << "m_sExecName:" << m_sExecName << endl;
    cout << "GetCurrDateTime:" << GetCurrDateTime() << endl;

    SLEEP(2);
    cout << "1 the num of thread is " << RThreadManager::Instance().GetCount() << endl;
    RThread::SetAllReload();
    SLEEP(5);
    for(int i=0; i < THREADS_NUM/2; ++i) 
    {
//        threads[i]->Exit();
        pthread_kill(threads[i]->GetThreadID(), SIGUSR1);
//        pthread_cancel(threads[i]->GetThreadID());
//        ret = threads[i]->Release();
        //*arg[i] = threads[i]->GetArg();
        cout << "threads[" << i << "] exec " << *arg[i] << " times" << endl;
//        delete arg[i];
//        if ( ret != true )
//            perror("status");
        //delete threads[i];
    }
    cout << "2 the num of thread is " << RThreadManager::Instance().GetCount() << endl;
    //RThread::SetAllExit();
    SLEEP(5);
    cout << "3 the num of thread is " << RThreadManager::Instance().GetCount() << endl;
    return 0;
}
