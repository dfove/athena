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

//记录输出文件的处理类
class ATHENA_EX RRecordFile
{
public:
    RRecordFile();
    /*
     *  params:
     *      filePath    记录输出目的文件
     *      igranularity    多长时间关闭文件(秒)
     *      formalFilePath  正式文件名，如果不为空的话，则关闭记录输出目的文件之后，将目的文件名改为正式文件名
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
    RString m_filePath;         //输出文件名

    RString m_formalFilePath;   //正式文件名

    time_t  m_createTime;       //文件创建时间or文件打开时间

    time_t  m_checkTime;        //检测文件的最后一次时间,每隔一段时间检查一下文件是否存在(默认120秒)

    int     m_granularity;      //关闭文件粒度(秒)

    FILE*   m_pFile;            //文件流指针

    bool    m_bPrototype;  //是否原型,true:prototype;false:clone

};

/*************************************************************************************
* RRecordFileList    -　记录文件链表类
* 说明：　　存放记录的文件链表
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

    friend class RRecord;   //使RRecord对象可以使用此对象的锁
};


//记录、日志、话单等输出记录类的父类
class ATHENA_EX RRecord
{
public:
    RRecord();

    RRecord(const RString& strName,          //记录名称
            const RString& strDir = "./",    //目录
            const RString& strPrefix = "",   //前缀
            const RString& strSuffix = "",   //后缀
            bool  isMemBlock = false,        //是否内存块
            int iGranularity = 3600,         //粒度,默认每小时
            bool bWriteTempFile = false,     //是否先写成临时文件
            bool bFlush = true);

    RRecord(const RRecord& right);

    virtual ~RRecord();

//参数设置
public:
    void SetParams(const RString& strName,          //记录名称
                   const RString& strDir,           //目录
                   const RString& strPrefix = "",   //前缀
                   const RString& strSuffix = "",   //后缀
                   bool  isMemBlock = false,        //是否内存块
                   int iGranularity = 3600,         //粒度
                   bool bWriteTempFile = false,     //是否先写成临时文件
                   bool bFlush = true);             //输出后是否立即flush

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
     *  临时文件名，不含路径
     *      m_granularity时间段内记录输出到该文件中;间隔时间后，将临时文件名改为正式文件名
     *  默认：记录文件名为"tmp_${m_prefix}${m_name}时间(根据记录模式构造).$m_suffix”
     */
    virtual const RString GenerateTempFilename();

    /*
     *  正式文件名，不含路径
     *  默认：记录文件名为"${m_prefix}${m_name}时间(根据记录模式构造).$m_suffix”
     */
    virtual const RString GenerateFileName();

    /*
     *  文件路径
     *  生成文件路径，子类可继承使用算法(如按时间构造)确定某条记录的文件路径
     *  默认：对象初始化指定的路径名
     */
    virtual const RString GenerateDir();

    /*
     *  会更新记录时间
     */
    virtual void FormatStr(const char* lpszFormat, ...);  //构造记录m_sBuffer

    /*
     *  params:
     *      bUpdateTime 是否更新记录的时间
     */
    virtual void FormatTimeStr(bool bUpdateTime, const char* lpszFormat, ...);  //构造记录m_sBuffer

    virtual void FormatMem(const void* memBlock, size_t iLen);        //构造内存块m_mem

protected:
    void FromatVStr(const char* lpszFormat, va_list argList);

public:
    /*
     *  将当前记录输出到文件(单线程)
     *  '输出文件'、'检查文件是否需要关闭' 在一个线程中进行，需要对m_sMutex加锁
     *  params:
     *      blLockFileToWrite 文件级别锁，仅在多进程可能同时操作一个文件的时候需要
     *          true    不需要文件级别锁
     *          false   对输出文件加文件级别锁(fcntl)
     */
    virtual void SaveToFile(bool blLockFileToWrite = false);

    /*
     *  将当前记录输出到文件(多线程)
     *  '输出文件'、'检查文件是否需要关闭' 在多个线程中进行
     */
    void MutThreadSaveToFile()
    {
        SaveToFile(true);
    }

//多线程加入列表，由单独的线程输出
public:
    virtual RRecord* Clone() const;

    void PushToList(RRecord* rec = NULL) const;            //将当前记录Clone后插入RRecordList列表

protected:
    RString m_name;     //记录名称

    RString m_dir;      //记录文件所在目录

    RString m_prefix;   //前缀

    RString m_suffix;   //后缀

    bool    m_isMem;    //是否内存块，否则为文本

    int m_granularity;  //记录粒度(秒)，每隔多长时间关闭打开一个新文件

    bool   m_bWriteTempFile;   //是否先输出到临时文件

    bool   m_bFlush;     //是否立即输出

    RTime  m_time;     //记录产生的时间

    RString m_sBuffer;  //m_isMemoryBlock==false记录内容

    RStream m_mem;   //m_isMemoryBlock==true内存块

private:
    RRecordFile*    m_recordFile;   //本记录输出目的文件的相关信息及其操作，原型RRecordFile指针

    bool            m_bPrototype;  //RRecord对象是否原型,true:prototype;false:clone

};

/*************************************************************************************
* RRecordList    -　记录链表类(也可以用来记录日志等)
* 说明：　　存放记录链表，以待处理(这里多采用多线程方式输出到文件)
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

    void Swap(std::list<RRecord*>& list);    //得到日志数据复本,清除当前链表数据

    void SaveAllToFile(void);                //将所有记录链表中信息存入文件

};

}   //namespace

#endif /*RRECORD_HPP_HEADER_INCLUDED*/
