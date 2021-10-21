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

#ifndef RTHREAD_HPP_HEADER_INCLUDED
#define RTHREAD_HPP_HEADER_INCLUDED

#include <Unidef.hpp>
#include <RString.hpp>
#include <RSMutex.hpp>
#include <vector>

namespace athena
{

struct RThread_t;

class ATHENA_EX RThread
{
public:
    typedef void* (*Start_Routine)(void* arg);

    typedef void* (*Start_Routine_SelfLoop)(RThread & self, void * arg);

    enum
    {
        Thr_Default = 0,                //默认为非分离线程、本进程争用CPU
        Thr_Detach  = 1,                //是否与父线程分离
        Thr_Globe   = 2                 //是否全局争用CPU
    };

public:
    RThread(const char* lpszname = NULL, bool bAutoDelete = true, bool bCanReload = true);

    virtual ~RThread();

public://子类需实现的操作
    virtual bool BeforeExec();          //执行线程启动之前初始化动作

    virtual bool BeforeReload();        //执行线程重新装载之前的初始化动作

    virtual void* Exec();               //默认执行线程

    virtual bool AfterExec();           //执行线程退出之后销毁动作

public://线程起停重载操作
    /*
     *  创建默认线程，默认线程执行Exec()函数，需要用户重写Exec()等虚函数
     *  params:
     *      looptimes   执行Exec函数的次数,-1为无限循环
     *      detached    是否分离线程,默认为非分离线程,父线程需要(join)等待子线程结束
     *      boundlwp    建立线程的争用范围，默认为进程内与其它线程争用
     */
    bool Create(long looptimes = -1, bool detached = false, bool boundlwp = false);

    /*
     *  创建默认的分离线程、全局争用、循环一次线程(用户实现无限循环)，需要用户重写Exec()虚函数
     *  params:
     *      thrType: Thr_Detach、Thr_Globe组合
     */
    bool Create(int thrType);

    /*
     *  创建用户自定义线程，线程执行用户传入的线程函数start_routine,其参数为arg
     *  params:
     *      start_routine   用户定义的线程函数，函数不执行无限循环
     *      arg             start_routine的参数
     *      looptimes       start_routine的执行次数，-1为无限循环
     *      detached        是否分离线程,默认为非分离线程,父线程需要(join)等待子线程结束
     *      boundlwp        建立线程的争用范围，默认为进程内与其它线程争用
     */
    bool Create(void*(*start_routine)(void *), void* arg, long looptimes = -1,
                bool detached = false, bool boundlwp = false);

    /*
     *  创建用户自定义线程，线程执行用户传入的无限循环线程函数start_routine_selfloop,其参数为arg
     *  params:
     *      start_routine   用户定义的线程函数，函数不执行无限循环，需要用户设置线程取消点
     *      arg             start_routine的参数
     *      looptimes       start_routine的执行次数，-1为无限循环
     *      detached        是否分离线程,默认为非分离线程,父线程需要(join)等待子线程结束SetCancelPoint()
     *      boundlwp        建立线程的争用范围，默认为进程内与其它线程争用
     */
    bool Create(void*(*start_routine_selfloop)(RThread &, void *), void* arg, long looptimes = -1,
                bool detached = false, bool boundlwp = false);

    /*
     *  在每个无限循环中调用，用以检查是否有退出事件或重载事件发生，即一个可退出、重载点
     *  仅当取消操作安全时才应取消线程，注意不要在无法释放资源的情况下（如已获取锁）设置取消点
     */
    void  SetCancelPoint();

    bool Reload();

    bool Exit();

    /*
     *  如果是joinable线程，则需要调用Join()等待线程结束后回收线程资源
     *    对于detached线程，调用此函数无影响
     */
    bool Release();

public://功能函数
    unsigned long GetThreadID() const;                  //得到本线程对象的线程ID

    const RString& GetName() const;                     //得到线程名

    unsigned long GetParentID() const;                  //得到父线程ID

    bool IsParent(unsigned long parentID) const;        //判断parentID是否本线程的父线程ID

    bool IsDetached() const;

    bool IsAutoDelete(void) const;                      //用于判断是否自动删除

public://与线程相关的全局操作函数
    static unsigned long GetCurrentThreadId();    //得到当前线程的ID号

    static size_t  GetThreadCount(void);          //得到线程总数目

    static void  SetAllExit(void);                      //设置所有线程退出

    static void  SetAllReload(void);                    //设置所有线程重载

    static RThread* GetThreadByName(const RString& thrName);    //根据进程名找到相应的进程

    static RThread* GetSelfPThread(void);               //得到当前线程所对应的线程类

    static size_t GetChildCount(void);                  //得到子线程数

    static bool Release(unsigned long dwID);                    //等待非分离线程退出，以释放资源

    static void Terminate(RThread* pThread);            //终止线程(仅能在线程外部调用以终止线程)

private:
    static void* Execution(void* arg);

    RThread(const RThread& other);

    const RThread& operator=(const RThread& other);

private:
    struct RThread_t*   m_thr;                          //线程标识

    unsigned long       m_parentID;                     //父线程ID

    RString             m_name;                         //线程名

    bool                m_bAutoDelete;                  //是否自动删除

    /*
     *  m_bRun: 线程状态
     *      0   未启动或退出
     *      1   正在运行
     */
    bool                m_bRun;                         //是否运行

    bool                m_bCanReload;                   //是否允许重载

    bool                m_bReload;                      //是否重载

    long                m_loopTimes;                    //循环次数

    bool                m_detached;                     //是否分离线程

protected:
    /*
     *  判断是否用户传入执行函数指针
     *      如果是默认的，则执行this->Exec()函数，用户可以重写该虚函数
     *      如果是用户传入的函数，则m_userDefinedExecution为执行函数指针
     *                              m_threadArg为执行函数参数
     */
    bool                m_defaultExecution;

    Start_Routine       m_userDefinedExecution;     //用户传入的执行函数指针

    /*
     *  用户传入的无限循环的执行函数指针，需要通过m_bRun判断是否退出
     */
    Start_Routine_SelfLoop  m_userDefinedLoopExecution; 

    void*               m_threadArg;                //用户传入的执行函数参数

};

/*************************************************************************************
* RThreadManager    -　线程管理类
* 说明：　　提供对所有从RThread派生类的管理
* 使用了以线程ID为索引的递增数组
**************************************************************************************/
class ATHENA_EX RThreadManager :public RSafeEx< std::vector<RThread*> >
{
protected:
    RThreadManager(void);

    RThreadManager(const RThreadManager& other);

    RThreadManager& operator = (const RThreadManager& other);

public:
    typedef std::vector<RThread*>::iterator PThread_Iterator;

    typedef std::vector<RThread*>::reverse_iterator R_PThread_Iterator;

public:
    static RThreadManager& Instance(void);

    void AddThread(RThread* pThread);        //增加一个新的线程

    void ExitThread(RThread* pThread);       //删除已退出的线程

    size_t GetCount(void);                   //得到当前的线程数

    void SetAllExit(void);                   //所有线程退出

    void SetAllReload(void);                 //所有线程重载

    RThread* GetFromID(unsigned long dwID);      //根据线程ID得到对应原线程类指针

    RThread* GetFromName(const RString& thrName);      //根据线程名得到对应原线程类指针

    RThread* GetParent(void);               //得到父线程对应的线程类指针

    size_t GetChildCount(void);             //得到其子线程数目

    bool CheckExist(RThread* pThread);	    //是否存在此线程

    bool CheckExist(unsigned long dwID);    //是否存在此线程

};


}   /*namespace athena*/

#endif /* RTHREAD_HPP_HEADER_INCLUDE */
