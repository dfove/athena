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
        Thr_Default = 0,                //Ĭ��Ϊ�Ƿ����̡߳�����������CPU
        Thr_Detach  = 1,                //�Ƿ��븸�̷߳���
        Thr_Globe   = 2                 //�Ƿ�ȫ������CPU
    };

public:
    RThread(const char* lpszname = NULL, bool bAutoDelete = true, bool bCanReload = true);

    virtual ~RThread();

public://������ʵ�ֵĲ���
    virtual bool BeforeExec();          //ִ���߳�����֮ǰ��ʼ������

    virtual bool BeforeReload();        //ִ���߳�����װ��֮ǰ�ĳ�ʼ������

    virtual void* Exec();               //Ĭ��ִ���߳�

    virtual bool AfterExec();           //ִ���߳��˳�֮�����ٶ���

public://�߳���ͣ���ز���
    /*
     *  ����Ĭ���̣߳�Ĭ���߳�ִ��Exec()��������Ҫ�û���дExec()���麯��
     *  params:
     *      looptimes   ִ��Exec�����Ĵ���,-1Ϊ����ѭ��
     *      detached    �Ƿ�����߳�,Ĭ��Ϊ�Ƿ����߳�,���߳���Ҫ(join)�ȴ����߳̽���
     *      boundlwp    �����̵߳����÷�Χ��Ĭ��Ϊ�������������߳�����
     */
    bool Create(long looptimes = -1, bool detached = false, bool boundlwp = false);

    /*
     *  ����Ĭ�ϵķ����̡߳�ȫ�����á�ѭ��һ���߳�(�û�ʵ������ѭ��)����Ҫ�û���дExec()�麯��
     *  params:
     *      thrType: Thr_Detach��Thr_Globe���
     */
    bool Create(int thrType);

    /*
     *  �����û��Զ����̣߳��߳�ִ���û�������̺߳���start_routine,�����Ϊarg
     *  params:
     *      start_routine   �û�������̺߳�����������ִ������ѭ��
     *      arg             start_routine�Ĳ���
     *      looptimes       start_routine��ִ�д�����-1Ϊ����ѭ��
     *      detached        �Ƿ�����߳�,Ĭ��Ϊ�Ƿ����߳�,���߳���Ҫ(join)�ȴ����߳̽���
     *      boundlwp        �����̵߳����÷�Χ��Ĭ��Ϊ�������������߳�����
     */
    bool Create(void*(*start_routine)(void *), void* arg, long looptimes = -1,
                bool detached = false, bool boundlwp = false);

    /*
     *  �����û��Զ����̣߳��߳�ִ���û����������ѭ���̺߳���start_routine_selfloop,�����Ϊarg
     *  params:
     *      start_routine   �û�������̺߳�����������ִ������ѭ������Ҫ�û������߳�ȡ����
     *      arg             start_routine�Ĳ���
     *      looptimes       start_routine��ִ�д�����-1Ϊ����ѭ��
     *      detached        �Ƿ�����߳�,Ĭ��Ϊ�Ƿ����߳�,���߳���Ҫ(join)�ȴ����߳̽���SetCancelPoint()
     *      boundlwp        �����̵߳����÷�Χ��Ĭ��Ϊ�������������߳�����
     */
    bool Create(void*(*start_routine_selfloop)(RThread &, void *), void* arg, long looptimes = -1,
                bool detached = false, bool boundlwp = false);

    /*
     *  ��ÿ������ѭ���е��ã����Լ���Ƿ����˳��¼��������¼���������һ�����˳������ص�
     *  ����ȡ��������ȫʱ��Ӧȡ���̣߳�ע�ⲻҪ���޷��ͷ���Դ������£����ѻ�ȡ��������ȡ����
     */
    void  SetCancelPoint();

    bool Reload();

    bool Exit();

    /*
     *  �����joinable�̣߳�����Ҫ����Join()�ȴ��߳̽���������߳���Դ
     *    ����detached�̣߳����ô˺�����Ӱ��
     */
    bool Release();

public://���ܺ���
    unsigned long GetThreadID() const;                  //�õ����̶߳�����߳�ID

    const RString& GetName() const;                     //�õ��߳���

    unsigned long GetParentID() const;                  //�õ����߳�ID

    bool IsParent(unsigned long parentID) const;        //�ж�parentID�Ƿ��̵߳ĸ��߳�ID

    bool IsDetached() const;

    bool IsAutoDelete(void) const;                      //�����ж��Ƿ��Զ�ɾ��

public://���߳���ص�ȫ�ֲ�������
    static unsigned long GetCurrentThreadId();    //�õ���ǰ�̵߳�ID��

    static size_t  GetThreadCount(void);          //�õ��߳�����Ŀ

    static void  SetAllExit(void);                      //���������߳��˳�

    static void  SetAllReload(void);                    //���������߳�����

    static RThread* GetThreadByName(const RString& thrName);    //���ݽ������ҵ���Ӧ�Ľ���

    static RThread* GetSelfPThread(void);               //�õ���ǰ�߳�����Ӧ���߳���

    static size_t GetChildCount(void);                  //�õ����߳���

    static bool Release(unsigned long dwID);                    //�ȴ��Ƿ����߳��˳������ͷ���Դ

    static void Terminate(RThread* pThread);            //��ֹ�߳�(�������߳��ⲿ��������ֹ�߳�)

private:
    static void* Execution(void* arg);

    RThread(const RThread& other);

    const RThread& operator=(const RThread& other);

private:
    struct RThread_t*   m_thr;                          //�̱߳�ʶ

    unsigned long       m_parentID;                     //���߳�ID

    RString             m_name;                         //�߳���

    bool                m_bAutoDelete;                  //�Ƿ��Զ�ɾ��

    /*
     *  m_bRun: �߳�״̬
     *      0   δ�������˳�
     *      1   ��������
     */
    bool                m_bRun;                         //�Ƿ�����

    bool                m_bCanReload;                   //�Ƿ���������

    bool                m_bReload;                      //�Ƿ�����

    long                m_loopTimes;                    //ѭ������

    bool                m_detached;                     //�Ƿ�����߳�

protected:
    /*
     *  �ж��Ƿ��û�����ִ�к���ָ��
     *      �����Ĭ�ϵģ���ִ��this->Exec()�������û�������д���麯��
     *      ������û�����ĺ�������m_userDefinedExecutionΪִ�к���ָ��
     *                              m_threadArgΪִ�к�������
     */
    bool                m_defaultExecution;

    Start_Routine       m_userDefinedExecution;     //�û������ִ�к���ָ��

    /*
     *  �û����������ѭ����ִ�к���ָ�룬��Ҫͨ��m_bRun�ж��Ƿ��˳�
     */
    Start_Routine_SelfLoop  m_userDefinedLoopExecution; 

    void*               m_threadArg;                //�û������ִ�к�������

};

/*************************************************************************************
* RThreadManager    -���̹߳�����
* ˵���������ṩ�����д�RThread������Ĺ���
* ʹ�������߳�IDΪ�����ĵ�������
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

    void AddThread(RThread* pThread);        //����һ���µ��߳�

    void ExitThread(RThread* pThread);       //ɾ�����˳����߳�

    size_t GetCount(void);                   //�õ���ǰ���߳���

    void SetAllExit(void);                   //�����߳��˳�

    void SetAllReload(void);                 //�����߳�����

    RThread* GetFromID(unsigned long dwID);      //�����߳�ID�õ���Ӧԭ�߳���ָ��

    RThread* GetFromName(const RString& thrName);      //�����߳����õ���Ӧԭ�߳���ָ��

    RThread* GetParent(void);               //�õ����̶߳�Ӧ���߳���ָ��

    size_t GetChildCount(void);             //�õ������߳���Ŀ

    bool CheckExist(RThread* pThread);	    //�Ƿ���ڴ��߳�

    bool CheckExist(unsigned long dwID);    //�Ƿ���ڴ��߳�

};


}   /*namespace athena*/

#endif /* RTHREAD_HPP_HEADER_INCLUDE */
