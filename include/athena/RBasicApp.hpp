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

#ifndef RBASICAPP_HPP_HEADER_INCLUDED
#define RBASICAPP_HPP_HEADER_INCLUDED

#include <Unidef.hpp>
#include <RString.hpp>
#include <vector>

namespace athena
{

class RThread;

enum exe_cmd
{
    CMD_Start   = 0,        //启动
    CMD_Stop    = 1,        //正常终止
    CMD_Abort   = 2,        //强行退出
    CMD_Reload  = 3,        //重载
    CMD_Restart = 4,        //stop,clean,start
    CMD_Clean   = 5,        //清理全局系统资源
    CMD_Stat    = 6,        //显示程序运行状态
    CMD_Help    = 7,        //显示帮助信息
};

static const char CMDInfo[9][8] = 
{
    "start",    //启动
    "stop",     //正常终止
    "abort",    //强行退出
    "reload",   //重载
    "restart",  //stop,clean,start
    "clean",    //清理全局系统资源
    "stat",     //显示程序运行状态
    "help"      //显示帮助信息
};

class ATHENA_EX RBasicApp
{
protected:
    RBasicApp(int argc = 1, char** argv = 0);

public:
    static RBasicApp* Instance(int argc = 1, char** argv = NULL);       //得到唯一工程类指针，子类必须重写此singleton方法

    virtual ~RBasicApp(){};

/*
 *  操作函数
 */
public:
    virtual int Execute();              //执行函数

protected:
    virtual int Start() = 0;            //命令参数为start时调用

    virtual int Stop();                 //默认kill

    virtual int Abort();                //默认kill -9 

    virtual int Reload();               //重载

    virtual int Restart();              //命令参数为restart是调用

    virtual int Clean();                //资源清理

    virtual int Stat();                 //运行状态

    virtual int Help();                 //默认参数提示,如有其它自定义参数可以重载此函数，进行解析，并且注意修改帮助说明

/*
 *  信号处理，如果子类重载应在重载函数中调用
 */
protected:
    virtual void WaitOver();            //当确定其它工作'线程or进程'已经正常退出后退出

    virtual void InitSignal();          //设置信号处理函数

    static  void RecvSignal(int sig);   //信号处理函数

    virtual bool HandleSignal(int sig); //返回FALSE时退出

    virtual void HandleReload();        //收到reload信号后所作的处理

    virtual void HandleExit();          //收到退出信号后所作的处理,如确定退出则返回TRUE，如不退出则返回TRUE

/*
 *  功能函数
 */
protected:
    virtual void InitDeamon();          //初始化为守进程

    virtual void InitParams();          //初始化对象参数

    virtual void InitConfig();          //读取配置文件，读取m_sConfFile,设置m_sLogDir等

public:
//    bool CreateWorkProcess(RProcess* pProcess); //创建一个工作进程

    bool CreateWorkThread(RThread* pThread);    //创建一个工作线程

    RString& ExecName();                        //得到程序名（除去路径及扩展名）

    std::vector<int>  GetPidFromName(const RString& sRunParam = "start");        //得到当前以参数sR正在运行的程序PID(除当前进程外)

protected:
    int                 m_uCmd;         //何种命令参数         

    std::vector<RString>     m_vsParam; //参数列表

    RString             m_sExecDir;     //程序所在目录

    RString             m_sExecName;    //程序名

    RString             m_sConfFile;    //配置文件名

    RString             m_sLogDir;      //日志目录名

    volatile int        m_nSignal;      //收到的信号值

    static RBasicApp*   m_pApp;         //存储全局唯一工程对象指针
};

}   /*namespace athena*/

#endif  /*RBASICAPP_HPP_HEADER_INCLUDED*/
