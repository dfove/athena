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
    CMD_Start   = 0,        //����
    CMD_Stop    = 1,        //������ֹ
    CMD_Abort   = 2,        //ǿ���˳�
    CMD_Reload  = 3,        //����
    CMD_Restart = 4,        //stop,clean,start
    CMD_Clean   = 5,        //����ȫ��ϵͳ��Դ
    CMD_Stat    = 6,        //��ʾ��������״̬
    CMD_Help    = 7,        //��ʾ������Ϣ
};

static const char CMDInfo[9][8] = 
{
    "start",    //����
    "stop",     //������ֹ
    "abort",    //ǿ���˳�
    "reload",   //����
    "restart",  //stop,clean,start
    "clean",    //����ȫ��ϵͳ��Դ
    "stat",     //��ʾ��������״̬
    "help"      //��ʾ������Ϣ
};

class ATHENA_EX RBasicApp
{
protected:
    RBasicApp(int argc = 1, char** argv = 0);

public:
    static RBasicApp* Instance(int argc = 1, char** argv = NULL);       //�õ�Ψһ������ָ�룬���������д��singleton����

    virtual ~RBasicApp(){};

/*
 *  ��������
 */
public:
    virtual int Execute();              //ִ�к���

protected:
    virtual int Start() = 0;            //�������Ϊstartʱ����

    virtual int Stop();                 //Ĭ��kill

    virtual int Abort();                //Ĭ��kill -9 

    virtual int Reload();               //����

    virtual int Restart();              //�������Ϊrestart�ǵ���

    virtual int Clean();                //��Դ����

    virtual int Stat();                 //����״̬

    virtual int Help();                 //Ĭ�ϲ�����ʾ,���������Զ�������������ش˺��������н���������ע���޸İ���˵��

/*
 *  �źŴ��������������Ӧ�����غ����е���
 */
protected:
    virtual void WaitOver();            //��ȷ����������'�߳�or����'�Ѿ������˳����˳�

    virtual void InitSignal();          //�����źŴ�����

    static  void RecvSignal(int sig);   //�źŴ�����

    virtual bool HandleSignal(int sig); //����FALSEʱ�˳�

    virtual void HandleReload();        //�յ�reload�źź������Ĵ���

    virtual void HandleExit();          //�յ��˳��źź������Ĵ���,��ȷ���˳��򷵻�TRUE���粻�˳��򷵻�TRUE

/*
 *  ���ܺ���
 */
protected:
    virtual void InitDeamon();          //��ʼ��Ϊ�ؽ���

    virtual void InitParams();          //��ʼ���������

    virtual void InitConfig();          //��ȡ�����ļ�����ȡm_sConfFile,����m_sLogDir��

public:
//    bool CreateWorkProcess(RProcess* pProcess); //����һ����������

    bool CreateWorkThread(RThread* pThread);    //����һ�������߳�

    RString& ExecName();                        //�õ�����������ȥ·������չ����

    std::vector<int>  GetPidFromName(const RString& sRunParam = "start");        //�õ���ǰ�Բ���sR�������еĳ���PID(����ǰ������)

protected:
    int                 m_uCmd;         //�����������         

    std::vector<RString>     m_vsParam; //�����б�

    RString             m_sExecDir;     //��������Ŀ¼

    RString             m_sExecName;    //������

    RString             m_sConfFile;    //�����ļ���

    RString             m_sLogDir;      //��־Ŀ¼��

    volatile int        m_nSignal;      //�յ����ź�ֵ

    static RBasicApp*   m_pApp;         //�洢ȫ��Ψһ���̶���ָ��
};

}   /*namespace athena*/

#endif  /*RBASICAPP_HPP_HEADER_INCLUDED*/
