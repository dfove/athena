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

#include <RBasicApp.hpp>
#include <RThread.hpp>
#include <RTime.hpp>
#include <RTools.hpp>
#include <signal.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

namespace athena
{
#if defined(WIN32) || defined(_WINDOWS)
    #define SIGRELOAD   SIGBREAK //�����ź�
#else
    #define SIGRELOAD   SIGUSR1 //�����ź�
#endif
}

using namespace athena;

RBasicApp* RBasicApp::m_pApp = 0;

RBasicApp::RBasicApp(int argc /*= 1*/, char** argv /*= 0*/)
          :m_uCmd(CMD_Help),
           m_nSignal(0)
{
    RString sFilePath(GetExeFilePath());//�õ������ȫ·����
    m_sExecDir = GetParentDirPath(sFilePath);//�õ�����Ŀ¼
    m_sExecName = GetNameFromFullPath(sFilePath);//�õ�������
    for(int i=1; i<argc; i++) //ȥ���������������������
    {
        m_vsParam.push_back(RString(argv[i]).MakeLower());
    }
    if ( m_vsParam.size() == 0 )
        m_vsParam.push_back("start");
}

RBasicApp* 
RBasicApp::Instance(int argc /*= 1*/, char** argv /*= NULL*/)
{
	return m_pApp;
}

int RBasicApp::Execute(void)
{
    int nRetn = 0;
    InitParams();
    if ( m_uCmd == CMD_Start )
    {
#ifndef _DEBUG
        InitDeamon();
#endif
        InitSignal();
        Start();
        WaitOver();
    }
    else if ( m_uCmd == CMD_Stop )
    {
        Stop();
    }
    else if ( m_uCmd == CMD_Abort )
    {
        Abort();
    }
    else if ( m_uCmd == CMD_Reload )
    {
        Reload();
    }
    else if ( m_uCmd == CMD_Restart )
    {
        Restart();
    }
    else if ( m_uCmd == CMD_Clean )
    {
        Clean();
    }
    else if ( m_uCmd == CMD_Stat )
    {
        Stat();
    }
    else if ( m_uCmd == CMD_Help )
    {
        Help();
    }
    return nRetn;
}

int 
RBasicApp::Stop()
{
    return 0;
}

int 
RBasicApp::Abort()
{
    return 0;
}

int 
RBasicApp::Reload()
{
    return 0;
}

int 
RBasicApp::Restart()
{
    return 0;
}

int 
RBasicApp::Clean()
{
    return 0;
}

int 
RBasicApp::Stat()
{
    return 0;
}

int 
RBasicApp::Help()
{
    printf("\nUsage: %s [start|stop|abort|reload|restart|clean|stat|help]\n\n", m_sExecName.c_str());
    printf("start [-c �����ļ�·��]\t����\n");
    printf("stop\t������ֹ\n");
    printf("abort\tǿ���˳�\n");
    printf("reload\t����\n");
    printf("restart\t����\n");
    printf("clean\t����ȫ��ϵͳ��Դ\n");
    printf("stat\t��ʾ��������״̬\n");
    printf("help\t��ʾ������Ϣ\n");
    printf("\n");
    return 0;
}

void 
RBasicApp::WaitOver(void)
{
    while (HandleSignal(m_nSignal))
    {
        ;
    }
}

void 
RBasicApp::InitSignal()
{
#if !defined(WIN32) && !defined(_WINDOWS)
    signal(SIGHUP, RecvSignal);
    signal(SIGPIPE,RecvSignal);
    signal(SIGPWR,RecvSignal);
    signal(SIGCHLD,RecvSignal);
    signal(SIGXFSZ,RecvSignal);
    signal(SIGALRM,RecvSignal);
    signal(SIGQUIT,RecvSignal);
#endif
    signal(SIGRELOAD,RecvSignal);
    signal(SIGTERM,RecvSignal);
    signal(SIGINT,RecvSignal);
    signal(SIGILL, RecvSignal);
}

void 
RBasicApp::RecvSignal(int sig)
{
    RASSERT(m_pApp);
    m_pApp->m_nSignal = sig;
    m_pApp->HandleSignal(sig);
    m_pApp->InitSignal();
}

bool 
RBasicApp::HandleSignal(int sig)
{
    switch(sig)
    {
        case 0:
            RTime::Sleep_s(1);
            return true;
        case SIGRELOAD:
            HandleReload();
            break;
        case SIGTERM:
#ifdef _DEBUG
        case SIGINT:
#endif
            HandleExit();
            return false;
        default:
            break;
    }
    m_nSignal = 0;
    return true;
}

void 
RBasicApp::HandleReload()
{
    RThread::SetAllReload();
}

void 
RBasicApp::HandleExit()
{
    RThread::SetAllExit();

    time_t last = time(NULL);
    while( RThread::GetThreadCount() != 0 )
    {//�ȴ������߳��˳�������RThreadManager������ɾ��
        time_t now = time(NULL);
        if ( now - last > 30)
        {//���30�뻹û����ȫ�˳������ٵ���һ�Σ�ֱ�������̶߳����˳����Ҵ�RThreadManager������ɾ��
            RThread::SetAllExit();
            last = now;
        }
        RTime::Sleep_r(1000);
    }
    exit(0);
}

void 
RBasicApp::InitDeamon()
{
#ifndef _WIN32
    pid_t pid = fork();
    if(pid > 0)
    {
        exit(0);
    }
    else if(pid < 0) 
    {
        exit(1);
    }

    setsid();

    pid = fork();
    if(pid > 0)
    {
        exit(0);
    }
    else if(pid < 0) 
    {
        exit(1);
    }

    //chdir("/");
    umask(0);
#endif
    return;
}

void 
RBasicApp::InitParams(void)
{
    std::vector<RString>::iterator it;
    it = find(m_vsParam.begin(), m_vsParam.end(), "-c");
    if(it != m_vsParam.end()) //�ҵ������ļ����ò���
    {
        it++;
        if(it != m_vsParam.end())
        {
            m_sConfFile = *it;
        }
        else
        {
            Help();
            exit(0);
        }
    }
    m_sConfFile = m_sExecDir ^ m_sConfFile;

    for(int i = 0; i <= CMD_Help; i++)
    {
        if ( m_vsParam.size() != 1 && ( i == 0 && m_vsParam.size() != 3 ))
        {
            Help();
            exit(0);
        }
        it = find(m_vsParam.begin(),m_vsParam.end(),RString(CMDInfo[i]));
        if(it != m_vsParam.end())
        {
            m_uCmd = i;
        }
    }
}

void 
RBasicApp::InitConfig()
{
}

//bool 
//RBasicApp::CreateWorkProcess(RProcess* pProcess)
//{
//    return false;
//}

bool 
RBasicApp::CreateWorkThread(RThread* pThread)
{
    if(!pThread->Create(RThread::Thr_Detach | RThread::Thr_Globe))
    {
        exit(-1);
    }
    return true;
}

inline RString& 
RBasicApp::ExecName()
{
    return m_sExecName;
}

std::vector<int>  RBasicApp::GetPidFromName(const RString& sRunParam)        //�õ����в���ΪsRunParam�ĵ�ǰ�������еĳ���PID(����ǰ������)
{
    std::vector<int> vPid;
    pid_t curpid = getpid();
    //�õ��������������У����������뵱ǰ������һ�£����������к���start�����ĳ���PID��Ϊ����
    RString sExecName,sParam;
    std::vector<RString> vsParam;

    DIR* dp;
    struct dirent *dirp;
    int pid;
    dp=opendir("/proc");
    chdir("/proc");

    while((dirp=readdir(dp))!=NULL)//�������еĽ���PID
    {
        RString sDir(dirp->d_name);
        if(!sDir.IsDigit())//�����ֵ�Ŀ¼����PID
        {
            continue;
        }

        pid = atoi(dirp->d_name);

        if(pid == curpid)
        {
            continue;
        }
        sDir.Format("/proc/%d",pid);//����ӳ��Ŀ¼

        {//��ȡ������
            RString sStat(sDir ^ "cmdline");
            ifstream cmdf(sStat.c_str());
            getline(cmdf,sParam,'\n');
            printf("%s\n", sParam.c_str());
            replace(sParam.begin(),sParam.end(),'\0',' ');
            vsParam = sParam.Strtok(" ");
            if(vsParam.size() < 2)
            {
                continue;
            }
            sExecName = GetNameFromFullPath(vsParam[0]);
        }

        if(m_sExecName == sExecName && find(vsParam.begin() + 1,vsParam.end(),sRunParam) != vsParam.end() )
        {       
            vPid.push_back(pid);
        }       
    }       
    closedir(dp);
    return vPid;
}
