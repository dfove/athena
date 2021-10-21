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

#ifndef PROCESS_HPP_HEADER_INCLUDED
#define PROCESS_HPP_HEADER_INCLUDED

#include <Unidef.hpp>
#include <RString.hpp>

namespace athena
{

#define G_ProcessMgr    (*RProcessMgr::Instance())

void OnTerminate(int nSig);

bool IsExited();

struct RProcItem
{
	char szName[20];		//名字
	int  nPID;			    //进程PID
	int  nCode;			    //进程的应用标识
	RProcItem*  pNext;
};

class RProcessMgr
{
public:
    static RProcessMgr* Instance();

	~RProcessMgr();

    /*
     *  params:
     *      pname   进程名称
     *      pid     进程PID
     *      code    进程应用标识
     *  return:
     *      bool    true:成功; false:失败
     */
    bool Add(const char* pname, int pid, int code);

    bool Add(const RProcItem &Item);

    /*
     *  清除 进程名为pname 的进程
     */
    void Delete(const char* pname);

    /*
     *  清除 应用标识为ncode 的进程
     */
    void Delete(int ncode);

	int Count() const;

	RProcItem& operator[](int nIndex);

    bool Find(const char* pname, int &nIndex) const;

    bool Find(int ncode, int &nIndex) const;

    void StopAllProcess();

private:
	RProcessMgr(); //禁止构造

private:
	int            m_count;

	RProcItem*     m_procItems;

    static RProcessMgr* m_instance;
};

class RProcess
{
public:
	RProcess(const RString& sName, int nCode);

	virtual ~RProcess();	

	int Execute();

protected:	
	virtual void BeforeRun();

	virtual void Run() = 0;

	virtual void AfterRun();

protected:
	RString m_sName;

	int m_nCode;        //进程在应用中的编号

	int m_nRetCode;     //退出时的返回值
};

}   /*namespace*/

#endif /*PROCESS_HPP_HEADER_INCLUDED*/
