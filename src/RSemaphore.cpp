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

#include <RSemaphore.hpp>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace athena
{
#if defined (WIN32) || defined (_WINDOWS)
    #include <windows.h>
    #include <limits.h>
    typedef struct
    {
        HANDLE h;
    }
    Sem_t;
#elif defined(HAVE_SEMAPHORE_H)
	#include <unistd.h>
    #include	<fcntl.h>		/* for nonblocking and O_RDWR | O_CREAT*/
    #define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP)
					    /* default permissions for new files 0x640*/
    #include <semaphore.h>
    typedef sem_t Sem_t;
#elif defined(HAVE_SYS_SEM_H)
	#include <unistd.h>
    #include <sys/sem.h>
    #define	SVSEM_MODE	0640
    #define MAX_TRIES   10
    typedef int Sem_t;
//    #if defined(_SEM_SEMUN_UNDEFINED)
        union semun
        {
             int val;                           /*<= value for SETVAL*/
             struct semid_ds *buf;              /*<= buffer for IPC_STAT & IPC_SET*/
             unsigned short int *array;         /*<= array for GETALL & SETALL*/
             struct seminfo *__buf;             /*<= buffer for IPC_INFO*/
        };
//    #endif
#endif
}

using namespace athena;

RSemaphore::RSemaphore(const char* name /*= NULL*/, unsigned int value /*= 0*/)
                      :m_sem(NULL)
{
    Sem_t *sem ;
    if ( name != NULL )
        m_name = name;
#if defined (WIN32) || defined (_WINDOWS)
    sem = new Sem_t;
    if((sem->h = CreateSemaphore(NULL, value, value+1, m_name)) == NULL)
    {
        delete sem;
        sem = NULL;
    }
    else if ( ERROR_ALREADY_EXISTS == GetLastError() )
    {
    }
#elif defined(HAVE_SEMAPHORE_H)
    if ( m_name != NULL )
    {
        if ( *name != '/' )
        {
            m_name.Format("%s%s", POSIX_IPC_PREFIX, name);
        }
        sem = sem_open(m_name, O_RDWR | O_CREAT, FILE_MODE, value);
        if ( sem == SEM_FAILED )
        {
            sem = NULL;
        }
    }
    else 
    {
        sem = new Sem_t;
        if(sem_init(sem, 0, value) != 0)
        {
            delete sem;
            sem = NULL;
        }
    }
#elif defined(HAVE_SYS_SEM_H)
    sem = new Sem_t;
    union semun arg;
    if ( m_name != NULL )
    {
        int oflag = IPC_CREAT | IPC_EXCL | SVSEM_MODE;
        if ( (*sem = semget(ftok(m_name, 0), 1, oflag)) >= 0 )    
            /*success, the frist so initialize.第二个参数1：共创建含多少信号灯的信号灯集,
                这里只创建有一个信号灯的信号灯集*/
        {
            arg.val = 2 * value;
            semctl(*sem, 0, SETVAL, arg);   //第二个参数0:第几个信号灯;这里只创建了有一个信号灯的信号灯集
            struct sembuf waitop;
            waitop.sem_num = 0;
            waitop.sem_op = -value;
            waitop.sem_flg = 0;
            semop(*sem, &waitop, 1);
        }
        else if ( errno == EEXIST )
            /*someone else has created, make sure it's initialized*/
        {
            *sem = semget(ftok(m_name, 0), 1, SVSEM_MODE);    //获取已存在的有名信号灯集
            struct semid_ds seminfo;
            arg.buf = &seminfo;
            for ( int i = 0; i < MAX_TRIES; i++)            //等待创建者初始化
            {
                semctl(*sem, 0, IPC_STAT, arg);
                if ( arg.buf->sem_otime != 0 )
                {
                    break;
                }
                sleep(1);
            }
        }
    }
    else
    {
        *sem = semget(IPC_PRIVATE, 1, IPC_CREAT | SVSEM_MODE);
        arg.val = value;
        semctl(*sem, 0, SETVAL, arg);
    }
    if ( *sem == -1 )
    {
        delete sem;
        sem = NULL;
    }
#endif
    m_sem = (struct RSem_t*)sem;
}

RSemaphore::~RSemaphore()
{
    if (m_sem != NULL)
    {
        Sem_t * sem = (Sem_t*)m_sem;
#if defined (WIN32) || defined (_WINDOWS)
        CloseHandle(sem->h);
        delete sem;
#elif defined(HAVE_SEMAPHORE_H)
        if ( m_name.empty() )
        {
//            sem_destroy(sem);
            delete sem;
        }
        else
        {
            sem_close(sem);
//            sem_unlink(m_name);
        }
#elif defined(HAVE_SYS_SEM_H)
//        semctl(*sem, 0, IPC_RMID);
        delete sem;
#endif
        sem = NULL;
        m_sem = NULL;
    }
}

bool 
RSemaphore::Wait()
{
    if(m_sem == NULL)
    {
        return false;
    }

    Sem_t * sem = (Sem_t*)m_sem;
#if defined (WIN32) || defined (_WINDOWS)
    DWORD err;
    if((err = WaitForSingleObject(sem->h, INFINITE)) != WAIT_OBJECT_0)
    {
        return false;
    }
#elif defined(HAVE_SEMAPHORE_H)
    int ret = sem_wait(sem);
    if ( ret != 0 )
        return false;
#elif defined (HAVE_SYS_SEM_H)
    struct sembuf waitop;
    waitop.sem_num = 0;
    waitop.sem_op = -1;
    waitop.sem_flg = 0;
    if ( semop(*sem, &waitop, 1) != 0 )
        return false;
#endif
    return true;
}

bool 
RSemaphore::TryWait()
{
    if(m_sem == NULL)
        return false;

    Sem_t * sem = (Sem_t*)m_sem;
#if defined (WIN32) || defined (_WINDOWS)
    DWORD err;
    if((err = WaitForSingleObject(sem->h, 0)) != WAIT_OBJECT_0)
    {
        return false;
    }
#elif defined(HAVE_SEMAPHORE_H)
    int ret = sem_trywait(sem);
    if ( ret != 0 )
        return false;
#elif defined(HAVE_SYS_SEM_H)
    struct sembuf waitop;
    waitop.sem_num = 0;
    waitop.sem_op = -1;
    waitop.sem_flg = IPC_NOWAIT | SEM_UNDO;
    if ( semop(*sem, &waitop, 1) != 0 )
        return false;
#endif
    return true;
}

bool 
RSemaphore::Post()
{
    if(m_sem == NULL)
        return false;

    Sem_t * sem = (Sem_t*)m_sem;
#if defined (WIN32) || defined (_WINDOWS)
    ReleaseSemaphore(sem->h, 1, NULL);
#elif defined(HAVE_SEMAPHORE_H)
    int ret = sem_post(sem);
    if ( ret != 0 )
        return false;
#elif defined(HAVE_SYS_SEM_H)
    struct sembuf postop;
    postop.sem_num = 0;
    postop.sem_op = 1;
    postop.sem_flg = SEM_UNDO;
    if ( semop(*sem, &postop, 1) != 0 )
        return false;
#endif
    return true;
}

bool 
RSemaphore::Remove()
{
    if (m_sem != NULL)
    {
        Sem_t * sem = (Sem_t*)m_sem;
#if defined (WIN32) || defined (_WINDOWS)
        /* The semaphore object is destroyed when its last handle has been closed.*/
        return true;
#elif defined(HAVE_SEMAPHORE_H)
        /*必须在确认没有进程（线程）在等待此信号量，否则删除会造成等待进程（线程）死锁*/
        if ( m_name.empty() )
        {
            if ( sem_destroy(sem) == -1 )
                return false;
        }
        else
        {
            if ( sem_unlink(m_name) == -1 )
                return false;
        }
#elif defined(HAVE_SYS_SEM_H)
        /*如果还有进程（线程）在等待此信号量，则删除此信号量，会解锁该进程（线程）*/
        if (semctl(*sem, 0, IPC_RMID) == -1)
            return false;
#endif
    }
    return true;
}

const RString& 
RSemaphore::GetName() const
{
    return m_name;
}

int  
RSemaphore::GetValue() const
{
    if(m_sem == NULL)
        return 0;

    Sem_t * sem = (Sem_t*)m_sem;
#if defined (WIN32) || defined (_WINDOWS)
    return 0;   //windows没有相关函数
#elif defined(HAVE_SEMAPHORE_H)
    int value = 0;
    int ret = sem_getvalue(sem, &value);
    return ret;
#elif defined(HAVE_SYS_SEM_H)
    return semctl(*sem, 0, GETNCNT);
#endif
}
