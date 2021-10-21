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

#ifndef RRECORD_HPP_HEADER_INCLUDED
#define RRECORD_HPP_HEADER_INCLUDED

#include <Unidef.hpp>
#include <RTime.hpp>
#include <RMutex.hpp>
#include <RSMutex.hpp>
#include <RString.hpp>
#include <RStream.hpp>
#include <RException.hpp>
#include <stdio.h>

#include <list>
#include <map>

namespace athena
{

//��¼����ļ��Ĵ�����
class ATHENA_EX RRecordFile
{
public:
    RRecordFile();
    /*
     *  params:
     *      filePath    ��¼���Ŀ���ļ�
     *      igranularity    �೤ʱ��ر��ļ�(��)
     *      formalFilePath  ��ʽ�ļ����������Ϊ�յĻ�����رռ�¼���Ŀ���ļ�֮�󣬽�Ŀ���ļ�����Ϊ��ʽ�ļ���
     */
    RRecordFile(const RString& filePath, int igranularity);

    RRecordFile(const RString& filePath, const RString& formalFilePath, int igranularity);

    RRecordFile(const RRecordFile& right);

    virtual ~RRecordFile();

public:
    virtual void PreCreateFile();

    virtual void AfterCloseFile();

    virtual RRecordFile* Clone() const;

    void OpenFile() throw(RException);

    void OpenFile(const RString& newFileName) throw(RException);

    void CloseFile();

    const RString& GetFilePath() const;

    void SetFormalFilePath(const RString& formalFilePath);

    const RString& GetFormalFilePath() const;

    time_t GetCreateTime() const;

    time_t GetCheckTime() const;

    void UpdateCheckTime();

    void SetGranularity(int granularity);

    int GetGranularity() const;

    FILE* GetFile() const;

private:
    RString m_filePath;         //����ļ���

    RString m_formalFilePath;   //��ʽ�ļ���

    time_t  m_createTime;       //�ļ�����ʱ��or�ļ���ʱ��

    time_t  m_checkTime;        //����ļ������һ��ʱ��,ÿ��һ��ʱ����һ���ļ��Ƿ����(Ĭ��120��)

    int     m_granularity;      //�ر��ļ�����(��)

    FILE*   m_pFile;            //�ļ���ָ��

    bool    m_bPrototype;  //�Ƿ�ԭ��,true:prototype;false:clone

};

/*************************************************************************************
* RRecordFileList    -����¼�ļ�������
* ˵����������ż�¼���ļ�����
**************************************************************************************/
class ATHENA_EX RRecordFileList:protected RSafeEx< std::list<RRecordFile*> >
{
protected:
    RRecordFileList(void);

    RRecordFileList(const RRecordFileList& recList);

    RRecordFileList& operator = (const RRecordFileList& recList);

public:
    static RRecordFileList& Instance(void);

    virtual ~RRecordFileList(void);

    bool IsEmpty(void);

    void Push_back(RRecordFile* pRecFile);

    RRecordFile* Find(const RString& filePath);

    void CheckTimeToCloseFile();

    friend class RRecord;   //ʹRRecord�������ʹ�ô˶������
};


//��¼����־�������������¼��ĸ���
class ATHENA_EX RRecord
{
public:
    RRecord();

    RRecord(const RString& strName,          //��¼����
            const RString& strDir = "./",    //Ŀ¼
            const RString& strPrefix = "",   //ǰ׺
            const RString& strSuffix = "",   //��׺
            bool  isMemBlock = false,        //�Ƿ��ڴ��
            int iGranularity = 3600,         //����,Ĭ��ÿСʱ
            bool bWriteTempFile = false,     //�Ƿ���д����ʱ�ļ�
            bool bFlush = true);

    RRecord(const RRecord& right);

    virtual ~RRecord();

//��������
public:
    void SetParams(const RString& strName,          //��¼����
                   const RString& strDir,           //Ŀ¼
                   const RString& strPrefix = "",   //ǰ׺
                   const RString& strSuffix = "",   //��׺
                   bool  isMemBlock = false,        //�Ƿ��ڴ��
                   int iGranularity = 3600,         //����
                   bool bWriteTempFile = false,     //�Ƿ���д����ʱ�ļ�
                   bool bFlush = true);             //������Ƿ�����flush

    void SetName(const RString& strName);

    void SetDir(const RString& strDir);

    void SetPrefix(const RString& strPrefix);

    void SetSuffix(const RString& strSuffix);

    void IsMemBlock(bool ifIsMem);

    void SetGranularity(int iGranularity);

    void SetIfWriteToTempFile(bool blWrite);

    void SetIfFlush(bool blFlush);

    void SetTime(time_t t);

    const RTime& GetTime() const;

    void SetRecordFileProc(RRecordFile* recFile);

public:
    /*
     *  ��ʱ�ļ���������·��
     *      m_granularityʱ����ڼ�¼��������ļ���;���ʱ��󣬽���ʱ�ļ�����Ϊ��ʽ�ļ���
     *  Ĭ�ϣ���¼�ļ���Ϊ"tmp_${m_prefix}${m_name}ʱ��(���ݼ�¼ģʽ����).$m_suffix��
     */
    virtual const RString GenerateTempFilename();

    /*
     *  ��ʽ�ļ���������·��
     *  Ĭ�ϣ���¼�ļ���Ϊ"${m_prefix}${m_name}ʱ��(���ݼ�¼ģʽ����).$m_suffix��
     */
    virtual const RString GenerateFileName();

    /*
     *  �ļ�·��
     *  �����ļ�·��������ɼ̳�ʹ���㷨(�簴ʱ�乹��)ȷ��ĳ����¼���ļ�·��
     *  Ĭ�ϣ������ʼ��ָ����·����
     */
    virtual const RString GenerateDir();

    /*
     *  ����¼�¼ʱ��
     */
    virtual void FormatStr(const char* lpszFormat, ...);  //�����¼m_sBuffer

    /*
     *  params:
     *      bUpdateTime �Ƿ���¼�¼��ʱ��
     */
    virtual void FormatTimeStr(bool bUpdateTime, const char* lpszFormat, ...);  //�����¼m_sBuffer

    virtual void FormatMem(const void* memBlock, size_t iLen);        //�����ڴ��m_mem

protected:
    void FromatVStr(const char* lpszFormat, va_list argList);

public:
    /*
     *  ����ǰ��¼������ļ�(���߳�)
     *  '����ļ�'��'����ļ��Ƿ���Ҫ�ر�' ��һ���߳��н��У���Ҫ��m_sMutex����
     *  params:
     *      blLockFileToWrite �ļ������������ڶ���̿���ͬʱ����һ���ļ���ʱ����Ҫ
     *          true    ����Ҫ�ļ�������
     *          false   ������ļ����ļ�������(fcntl)
     */
    virtual void SaveToFile(bool blLockFileToWrite = false);

    /*
     *  ����ǰ��¼������ļ�(���߳�)
     *  '����ļ�'��'����ļ��Ƿ���Ҫ�ر�' �ڶ���߳��н���
     */
    void MutThreadSaveToFile()
    {
        SaveToFile(true);
    }

//���̼߳����б��ɵ������߳����
public:
    virtual RRecord* Clone() const;

    void PushToList(RRecord* rec = NULL) const;            //����ǰ��¼Clone�����RRecordList�б�

protected:
    RString m_name;     //��¼����

    RString m_dir;      //��¼�ļ�����Ŀ¼

    RString m_prefix;   //ǰ׺

    RString m_suffix;   //��׺

    bool    m_isMem;    //�Ƿ��ڴ�飬����Ϊ�ı�

    int m_granularity;  //��¼����(��)��ÿ���೤ʱ��رմ�һ�����ļ�

    bool   m_bWriteTempFile;   //�Ƿ����������ʱ�ļ�

    bool   m_bFlush;     //�Ƿ��������

    RTime  m_time;     //��¼������ʱ��

    RString m_sBuffer;  //m_isMemoryBlock==false��¼����

    RStream m_mem;   //m_isMemoryBlock==true�ڴ��

private:
    RRecordFile*    m_recordFile;   //����¼���Ŀ���ļ��������Ϣ���������ԭ��RRecordFileָ��

    bool            m_bPrototype;  //RRecord�����Ƿ�ԭ��,true:prototype;false:clone

};

/*************************************************************************************
* RRecordList    -����¼������(Ҳ����������¼��־��)
* ˵����������ż�¼�����Դ�����(�������ö��̷߳�ʽ������ļ�)
**************************************************************************************/
class RRecordList:protected RSafe< std::list<RRecord*> >
{
protected:
    RRecordList(void);

    RRecordList(const RRecordList& recList);

    RRecordList& operator = (const RRecordList& recList);

public:
    static RRecordList& Instance(void);

    virtual ~RRecordList(void);

    bool IsEmpty(void);

    void Push_back(RRecord* pRecord);

    void Swap(std::list<RRecord*>& list);    //�õ���־���ݸ���,�����ǰ��������

    void SaveAllToFile(void);                //�����м�¼��������Ϣ�����ļ�

};

}   //namespace

#endif /*RRECORD_HPP_HEADER_INCLUDED*/
