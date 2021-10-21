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

#include <RProcess.hpp>
#include <RTime.hpp>

#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <exception>
#include <iostream>
#include <stdlib.h>

using namespace athena;
using namespace std;

/***********************************
进程管理类
***********************************/
RProcessMgr* RProcessMgr::m_instance = 0;
bool g_bExit = false;

void 
athena::OnTerminate(int nSig)
{
//    cout << "set g_bExit to true:" << getpid() << "(" << nSig << ")" << strsignal(nSig) << endl;
	g_bExit = true;
}

bool 
athena::IsExited()
{
    return g_bExit;
}

RProcessMgr* 
RProcessMgr::Instance()
{
    if ( m_instance == NULL )
        m_instance = new RProcessMgr();
    return m_instance;
}

RProcessMgr::RProcessMgr()
            :m_count(0),
             m_procItems(NULL)
{
}

RProcessMgr::~RProcessMgr()
{
    if ( m_procItems != NULL )
    {
        RProcItem* pItem = m_procItems;
        while(pItem)
        {
            RProcItem* ptemp = pItem;
            pItem = pItem->pNext;
            delete ptemp;
        }
    }
}

bool 
RProcessMgr::Add(const char* pname, int pid, int code)
{
    RProcItem* pItem = new RProcItem;
    memset(pItem, 0, sizeof(RProcItem));
    strncpy(pItem->szName, pname, sizeof(pItem->szName) - 1);
    pItem->nPID = pid;
    pItem->nCode = code;
    pItem->pNext = NULL;

    //get the last slot
    RProcItem* pTemp = m_procItems;
    if ( pTemp == NULL )
    {
        m_procItems = pItem;
        m_count = 1;
        return true;
    }

    while (pTemp->pNext)
        pTemp = pTemp->pNext;
    pTemp->pNext = pItem;
    m_count++;
    return true;
}

bool 
RProcessMgr::Add(const RProcItem &item)
{
    return Add(item.szName, item.nPID, item.nCode);
}

void 
RProcessMgr::Delete(const char* pname)
{
    RProcItem* pCur = m_procItems;
    RProcItem* pLast = NULL;
    while(pCur)
    {
        if (strcmp(pCur->szName, pname) == 0)
            break;
        pLast = pCur;
        pCur = pCur->pNext;
    }
    if ( !pCur )            //没找到
        return;

    if ( pLast == NULL)     //第一个
    {
        m_procItems = pCur->pNext;
        m_count--;
        delete pCur;
    }
    else
    {
        pLast->pNext = pCur->pNext;
        m_count--;
        delete pCur;
    }
}

void 
RProcessMgr::Delete(int ncode)
{
    RProcItem* pCur = m_procItems;
    RProcItem* pLast = NULL;
    while(pCur)
    {
        if (pCur->nCode == ncode)
            break;
        pLast = pCur;
        pCur = pCur->pNext;
    }
    if ( !pCur )            //没找到
        return;

    if ( pLast == NULL)     //第一个
    {
        m_procItems = pCur->pNext;
    }
    else
    {
        pLast->pNext = pCur->pNext;
    }
    m_count--;
    delete pCur;
}

int 
RProcessMgr::Count() const
{
	return m_count;
}

RProcItem& 
RProcessMgr::operator[](int nIndex)
{
    if ( nIndex < 0 || nIndex > m_count - 1 )
    {
//        gvLog(ERR, "invalid params, exceed to processmgr range[0-%d], current is %d!", m_count, nIndex);
        throw exception();
    }
    RProcItem* pCur = m_procItems;
    while(nIndex--)
    {
        pCur = pCur->pNext;
    }
    return *pCur;
}

bool 
RProcessMgr::Find(const char* pname, int &nIndex) const
{
    RProcItem* pCur = m_procItems;
    int n = 0;
    while(pCur)
    {
        n++;
        if (strcmp(pCur->szName, pname) == 0)
            break;
        pCur = pCur->pNext;
    }
    if ( !pCur )            //没找到
    {
        nIndex = -1;
        return false;
    }

    nIndex = n;
    return true;
}

bool 
RProcessMgr::Find(int ncode, int &nIndex) const
{
    RProcItem* pCur = m_procItems;
    int n = 0;
    while(pCur)
    {
        n++;
        if (pCur->nCode == ncode)
            break;
        pCur = pCur->pNext;
    }
    if ( !pCur )            //没找到
    {
        nIndex = -1;
        return false;
    }

    nIndex = n;
    return true;
}

void 
RProcessMgr::StopAllProcess()
{
    RProcItem* pCur = m_procItems;
    RTime::Sleep_s(1);;//防止进程还没有记录进程信息就退出
    while(pCur)
    {
#if defined(WIN32) || defined(_WINDOWS)
#else
        kill(pCur->nPID, SIGTERM);
#endif
        pCur = pCur->pNext;
    }
}


/***********************************
进程处理类
***********************************/
RProcess::RProcess(const RString& sName, int nCode):
	m_sName(sName),	
	m_nCode(nCode),
	m_nRetCode(0)
{
}

RProcess::~RProcess()
{
}

int 
RProcess::Execute()
{
	int id;

#if defined(WIN32) || defined(_WINDOWS)
    id = 0;
#else
	id = fork();
#endif
	if(id == 0)
	{
		try
		{
			BeforeRun();
			Run();
		}
		catch(exception &e)
		{
			m_nRetCode = errno;
//	        gvLog(ERR, "Child process %ld,%s catch exception:%s!", getpid(), m_sName.c_str(), e.what());
		}

		AfterRun();
	}
	else if(id > 0)
	{
		G_ProcessMgr.Add(m_sName.c_str(), id, m_nCode);
	}
	else if(id < 0)
	{
//        gvLog(ERR, "fork FAIL!!");
		throw exception();
	}

	return id;
}

void 
RProcess::BeforeRun()
{
	//忽略某些信号
	signal(SIGTERM, OnTerminate);
	signal(SIGUSR1, OnTerminate);
	signal(SIGUSR2, OnTerminate);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGURG, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);

//	gvLog(INFO, "Child process %ld, %s is procing...", getpid(), m_sName.c_str());
}

void 
RProcess::AfterRun()
{
//	gvLog(INFO, "Child process %ld, %s is end!", getpid(), m_sName.c_str());
	exit(m_nRetCode);
}
