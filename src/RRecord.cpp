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

#include <RRecord.hpp>
#include <RTools.hpp>
#include <RFileLock.hpp>

#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/stat.h>

using namespace athena;

/************************************
 *  记录输出目的文件
 ************************************/
RRecordFile::RRecordFile()
            :m_createTime(time(0)),
             m_checkTime(m_createTime),
             m_granularity(0),
             m_pFile(0),
             m_bPrototype(true)//prototype
{

}

RRecordFile::RRecordFile(const RString& filePath, int igranularity)
            :m_filePath(filePath),
             m_createTime(time(0)),
             m_checkTime(m_createTime),
             m_granularity(igranularity),
             m_pFile(0),
             m_bPrototype(true)//prototype
{
    OpenFile();
}

RRecordFile::RRecordFile(const RString& filePath, const RString& formalFilePath, int igranularity)
            :m_filePath(filePath),
             m_formalFilePath(formalFilePath),
             m_createTime(time(0)),
             m_checkTime(m_createTime),
             m_granularity(igranularity),
             m_pFile(0),
             m_bPrototype(true)//prototype
{
    OpenFile();
}

RRecordFile::RRecordFile(const RRecordFile& right)
            :m_filePath(right.m_filePath),
             m_formalFilePath(right.m_formalFilePath),
             m_createTime(right.m_createTime),
             m_checkTime(right.m_checkTime),
             m_granularity(right.m_granularity),
             m_pFile(right.m_pFile),
             m_bPrototype(false)//clone
{
    
}

RRecordFile::~RRecordFile()
{
    if ( m_bPrototype == false && m_pFile != NULL )
    {
        fclose(m_pFile);
        m_pFile = NULL;
        if ( !m_formalFilePath.empty() )
        {
            struct stat buf;
            if ( stat(m_formalFilePath.c_str(), &buf) == -1 )
            {
                rename(m_filePath.c_str(), m_formalFilePath.c_str());
            }
            else
            {
                stat(m_filePath.c_str(), &buf);
                int fileSize = buf.st_size;
                void* fileBuf = malloc(fileSize);
                FILE* fileSrc = fopen(m_filePath.c_str(), "r");
                fread(fileBuf, fileSize, 1, fileSrc);
                fclose(fileSrc);

                FILE* fileDst = fopen(m_formalFilePath.c_str(), "a");
                fwrite(fileBuf, fileSize, 1, fileDst);
                fclose(fileDst);

                free(fileBuf);
                unlink(m_filePath.c_str());
            }
        }
    }
}

void 
RRecordFile::PreCreateFile()
{

}

void 
RRecordFile::AfterCloseFile()
{

}

RRecordFile* 
RRecordFile::Clone() const
{
    return new RRecordFile(*this);
}

void 
RRecordFile::OpenFile() throw(RException)
{
    RASSERT(!m_filePath.empty());
    m_pFile = fopen(m_filePath.c_str(), "a");
    if ( NULL == m_pFile )
    {
        RString errMsg;
        errMsg.Format("open file [%s] error:%s!", m_filePath.c_str(), strerror(errno));
        throw(RException(errMsg));
    }
    m_checkTime = m_createTime = time(0);
}

void 
RRecordFile::OpenFile(const RString& newFileName) throw(RException)
{
    m_filePath = newFileName;
    OpenFile();
    m_checkTime = m_createTime = time(0);
}

void 
RRecordFile::CloseFile()
{
    if ( m_pFile == NULL )
        return;
    fclose(m_pFile);
    m_pFile = NULL;
}

const RString& 
RRecordFile::GetFilePath() const
{
    return m_filePath;
}

void 
RRecordFile::SetFormalFilePath(const RString& formalFilePath)
{
    m_formalFilePath = formalFilePath;
}

const RString& 
RRecordFile::GetFormalFilePath() const
{
    return m_formalFilePath;
}

time_t 
RRecordFile::GetCreateTime() const
{
    return m_createTime;
}

time_t 
RRecordFile::GetCheckTime() const
{
    return m_checkTime;
}

void 
RRecordFile::UpdateCheckTime()
{
    m_checkTime = time(NULL);
}

void 
RRecordFile::SetGranularity(int granularity)
{
    m_granularity = granularity;
}

int 
RRecordFile::GetGranularity() const
{
    return m_granularity;
}

FILE* 
RRecordFile::GetFile() const
{
    return m_pFile;
}


/*****************************************
* RRecordFileList    -　记录文件链表类
* 说明：　　存放记录的文件链表
******************************************/
RRecordFileList::RRecordFileList(void)
{

}

RRecordFileList& 
RRecordFileList::Instance(void)
{
    static RRecordFileList _instance;
    return _instance;
}

RRecordFileList::~RRecordFileList(void)
{
    WRLOCK(m_Mutex);
    std::list<RRecordFile*>::iterator iter, iCur;
    for ( iter = m_Data.begin(); iter != m_Data.end(); )
    {
        iCur = iter++;
        RRecordFile* pRecFile = *iCur;
        pRecFile->AfterCloseFile();
        delete pRecFile;
        m_Data.erase(iCur);
    }
}

bool 
RRecordFileList::IsEmpty(void)
{
    RDLOCK(m_Mutex);
    return m_Data.empty();
}

void 
RRecordFileList::Push_back(RRecordFile* pRecFile)
{
    WRLOCK(m_Mutex);
    std::list<RRecordFile*>::iterator iter;
    for ( iter = m_Data.begin(); iter != m_Data.end(); iter++)
    {
        RRecordFile* pTmp = *iter;
        if ( pTmp->GetFilePath() == pRecFile->GetFilePath() )
            return;
    }
//    printf("RRecordFileList::Push_back filelist is %ld, file:%s, line:%d, pRecFile->GetFilePath():%s\n", m_Data.size(), __FILE__, __LINE__, pRecFile->GetFilePath().c_str());
    m_Data.push_back(pRecFile);
}

void 
RRecordFileList::CheckTimeToCloseFile()
{
    time_t now = time(NULL);
    std::list<RRecordFile*>::iterator iter, iCur;
    WRLOCK(m_Mutex);
    for(iter = m_Data.begin(); iter != m_Data.end(); )
    {
        iCur = iter++;
        RRecordFile* pRecFile = *iCur;
        if ( ( now != pRecFile->GetCreateTime() && now % pRecFile->GetGranularity() == 0 ) || 
             now - pRecFile->GetCreateTime() > pRecFile->GetGranularity() )
        {
//            printf("RRecordFileList::CheckTimeToCloseFile() filelist is %ld, file:%s, line:%d\n", m_Data.size(), __FILE__, __LINE__);
            m_Data.erase(iCur);
            pRecFile->AfterCloseFile();
            delete pRecFile;//删掉RRecordFild对象会自动关闭文件
//            printf("RRecordFileList::CheckTimeToCloseFile() filelist is %ld, file:%s, line:%d\n", m_Data.size(), __FILE__, __LINE__);
        }
    }
}

RRecordFile* 
RRecordFileList::Find(const RString& filePath)
{//用于RRecord.SaveToFile找到对应的输出文件对象，用于输出记录
    std::list<RRecordFile*>::iterator iter;
    for(iter = m_Data.begin(); iter != m_Data.end(); iter++)
    {
        RRecordFile* pRecFile = *iter;
        if ( filePath == pRecFile->GetFilePath() )
            return pRecFile;
    }
    return 0;
}


/************************************
 *  记录
 ************************************/
RRecord::RRecord()
        :m_isMem(false),
         m_granularity(3600),
         m_bWriteTempFile(false),
         m_bFlush(true),
         m_recordFile(NULL),
         m_bPrototype(true)//prototype
{
    RASSERT(m_granularity != 0);
}

RRecord::RRecord(const RString& strName, 
                 const RString& strDir, 
                 const RString& strPrefix /* = "" */, 
                 const RString& strSuffix /* = "" */, 
                 bool  isMemBlock /* = false */,
                 int iGranularity /* = 3600 */, 
                 bool bWriteTempFile /* = false */, 
                 bool bFlush /* = true */)
        :m_name(strName),
         m_dir(strDir),
         m_prefix(strPrefix),
         m_suffix(strSuffix),
         m_isMem(isMemBlock),
         m_granularity(iGranularity),
         m_bWriteTempFile(bWriteTempFile),
         m_bFlush(bFlush),
         m_recordFile(NULL),
         m_bPrototype(true)//prototype
{
    m_time.GetCurrentTime();
}

RRecord::RRecord(const RRecord& right)
        :m_name(right.m_name),
         m_dir(right.m_dir),
         m_prefix(right.m_prefix),
         m_suffix(right.m_suffix),
         m_isMem(right.m_isMem),
         m_granularity(right.m_granularity),
         m_bWriteTempFile(right.m_bWriteTempFile),
         m_bFlush(right.m_bFlush),
         m_time(right.m_time),
         m_sBuffer(right.m_sBuffer),
         m_mem(right.m_mem),
         m_recordFile(right.m_recordFile),
         m_bPrototype(false)//clone
{
}

RRecord::~RRecord()
{
    if ( m_bPrototype == true && m_recordFile != NULL )
    {
//        printf("file:%s, line:%d, RRecord::~RRecord():m_sBuffer:%s\n", __FILE__, __LINE__, m_sBuffer.c_str());
        delete m_recordFile;
        m_recordFile = NULL;
    }
}

void 
RRecord::SetParams(const RString& strName, 
                   const RString& strDir, 
                   const RString& strPrefix /* = "" */, 
                   const RString& strSuffix /* = "" */, 
                   bool  isMemBlock /* = false */,
                   int iGranularity /* = 3600 */, 
                   bool bWriteTempFile /* = false */, 
                   bool bFlush /* = true */)
{
    m_name = strName ;
    m_dir = strDir;
    m_prefix = strPrefix;
    m_suffix = strSuffix;
    m_isMem = isMemBlock;
    m_granularity = iGranularity;
    m_bWriteTempFile = bWriteTempFile;
    m_bFlush = bFlush;
}

void 
RRecord::SetName(const RString& strName)
{
    m_name = strName;
}

void 
RRecord::SetDir(const RString& strDir)
{
    m_dir = strDir;
}

void 
RRecord::SetPrefix(const RString& strPrefix)
{
    m_prefix = strPrefix;
}

void 
RRecord::SetSuffix(const RString& strSuffix)
{
    m_suffix = strSuffix;
}

void 
RRecord::IsMemBlock(bool ifIsMem)
{
    m_isMem = ifIsMem;
}

void 
RRecord::SetGranularity(int iGranularity)
{
    m_granularity = iGranularity;
}

void 
RRecord::SetIfWriteToTempFile(bool blWrite)
{
    m_bWriteTempFile = blWrite;
}

void 
RRecord::SetIfFlush(bool blFlush)
{
    m_bFlush = blFlush;
}

void 
RRecord::SetTime(time_t t)
{
    m_time = RTime(t);
}

const RTime& 
RRecord::GetTime() const
{
    return m_time;
}

void 
RRecord::SetRecordFileProc(RRecordFile* recFile)
{
    m_recordFile = recFile;
}

const RString 
RRecord::GenerateTempFilename()
{
    RString strFileName = GenerateFileName();
    return "tmp_" + strFileName;
}

const RString 
RRecord::GenerateFileName()
{
    RString strFileName;
    if ( !m_prefix.empty() )
        strFileName += m_prefix;
    if ( !m_name.empty() )
        strFileName += m_name;
    RString strDate;
    if ( m_granularity >= 86400 )    //粒度大于等于一天
        strDate = m_time.GetDate();
    else if ( m_granularity >= 3600 )   //粒度大于等于一小时
    {
        strDate.Format("%s%02d", m_time.GetDate().c_str(), m_time.GetHour());
    }
    else if ( m_granularity >= 60 )     //粒度大于等于一分钟
        strDate.Format("%s%02d%02d", m_time.GetDate().c_str(), m_time.GetHour(), m_time.GetMinute());
    else    //粒度以秒为单位
    {
        time_t sec = ( m_time.GetTime_t() / m_granularity ) * m_granularity; //取整
        RTime revise_time(sec);
        strDate.Format("%s%02d%02d%02d", m_time.GetDate().c_str(), m_time.GetHour(), m_time.GetMinute(), revise_time.GetSecond());
    }
    strFileName += strDate;
    if ( !m_suffix.empty() )
    {
        strFileName += ".";
        strFileName += m_suffix;
    }
    return strFileName;
}

const RString 
RRecord::GenerateDir()
{
    struct stat buf;
    if ( stat(m_dir.c_str(), &buf) == -1 )
    {
        int ret = mkdir(m_dir.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        if ( ret != -1 )
        {
            RString strMkdirErr;
            strMkdirErr.Format("mkdir %s error!", m_dir.c_str());
            if ( m_isMem )
                FormatMem(strMkdirErr.c_str(), strMkdirErr.size());
            else
                FormatStr(strMkdirErr.c_str());
        }
    }
    return m_dir;
}

void 
RRecord::FormatStr(const char* lpszFormat,...)
{
    m_time.GetCurrentTime();
    RASSERT(lpszFormat != NULL);
    va_list ap;
    va_start(ap, lpszFormat);
    m_sBuffer.FormatV(lpszFormat,ap);
    va_end(ap);
}

void 
RRecord::FormatTimeStr(bool bUpdateTime, const char* lpszFormat, ...)
{
    RASSERT(lpszFormat != NULL);
    if ( bUpdateTime == true )
        m_time.GetCurrentTime();
    va_list ap;
    va_start(ap, lpszFormat);
    m_sBuffer.FormatV(lpszFormat,ap);
    va_end(ap);
}

void 
RRecord::FromatVStr(const char* lpszFormat, va_list argList)
{
    RASSERT(lpszFormat != NULL);
    m_sBuffer.FormatV(lpszFormat,argList);
}

void 
RRecord::FormatMem(const void* memBlock, size_t iLen)
{
    m_time.GetCurrentTime();
    RASSERT(memBlock != NULL);
    m_mem.Append(memBlock, iLen);
}

void 
RRecord::SaveToFile(bool blLockFileToWrite /*= false*/)
{
    RString strFullPathName;
    RString strFormalFileName;
    if ( m_bWriteTempFile == true )
    {
        strFullPathName = GenerateDir() ^ GenerateTempFilename();
        strFormalFileName = GenerateDir() ^ GenerateFileName();
    }
    else
        strFullPathName = GenerateDir() ^ GenerateFileName();

    FILE* pFile = 0;
    RRecordFileList& recFileList = RRecordFileList::Instance();
    RRecordFile* pRecFile = recFileList.Find(strFullPathName);
    if( pRecFile != NULL )
    {
        if( time(NULL) - pRecFile->GetCheckTime() >=120 ) 
        {//两分钟检查一次文件，如果文件被人为删除，重建它
            struct stat st;
            if(stat(strFullPathName.c_str(), &st) != 0)
            {
                pRecFile->CloseFile();
                pRecFile->OpenFile();
                if ( NULL == pRecFile->GetFile() )
                {
                    RString errMsg;
                    errMsg.Format("open file [%s] error:%s!", strFullPathName.c_str(), strerror(errno));
                    throw(RException(errMsg));
                }
            }
            pRecFile->UpdateCheckTime();
        }
        pFile = pRecFile->GetFile();
    }
    else
    {
//        pRecFile = new RRecordFile(strFullPathName, m_granularity);
//        recFileList.Push_back(pRecFile);
//        printf("this is RRecord::SaveToFile() 1\n");
//        pFile = pRecFile->GetFile();

        try
        {
            if ( m_recordFile == NULL )
            {//如果没设置文件处理方式，则使用默认方式
                if ( m_bWriteTempFile == true && !strFormalFileName.empty() )
                {
                    m_recordFile = new RRecordFile(strFullPathName, strFormalFileName, m_granularity);
                }
                else
                    m_recordFile = new RRecordFile(strFullPathName, m_granularity);
            }
            else
            {
                m_recordFile->OpenFile(strFullPathName);
                m_recordFile->SetGranularity(m_granularity);
                if ( m_bWriteTempFile == true && !strFormalFileName.empty() )
                    m_recordFile->SetFormalFilePath(strFormalFileName);
            }
        }
        catch(RException e)
        {
            m_isMem = false;
            strFullPathName = "./" ^ GenerateFileName();
            m_recordFile = new RRecordFile(strFullPathName, m_granularity);
            m_sBuffer.Format("%s", e.What().c_str());
        }
        m_recordFile->PreCreateFile();
//        printf("file:%s, line:%d, not to new RRecord::SaveToFile(), fullpathname:%s, buf:%s\n", __FILE__, __LINE__, strFullPathName.c_str(), m_sBuffer.c_str());
        recFileList.Push_back(m_recordFile->Clone());
        pFile = m_recordFile->GetFile();
    }

    RASSERT(pFile!=NULL);
    if ( blLockFileToWrite == true )
    {
        int fd = fileno(pFile);
        RSafeFWrLock fLock(fd);
        if ( m_isMem == false )
            fprintf(pFile, "%s\n", m_sBuffer.c_str());
        else
        {
            fwrite(m_mem.Data(), m_mem.Size(), 1, pFile);
        }
    }
    else
    {
        if ( m_isMem == false )
            fprintf(pFile, "%s\n", m_sBuffer.c_str());
        else
        {
            fwrite(m_mem.Data(), m_mem.Size(), 1, pFile);
        }
    }

    if(m_bFlush)
    {
        fflush(pFile);
    }
    return;
}

RRecord* 
RRecord::Clone() const
{
    return new RRecord(*this);
}

void 
RRecord::PushToList(RRecord* rec /* = NULL */) const
{
    RRecordList& ins = RRecordList::Instance();
    if ( rec == NULL )
        ins.Push_back(Clone());
    else
        ins.Push_back(rec);
}

/*************************************************************************************
* RRecordList    -　记录链表类实现(也可以用来记录日志等)
* 说明：　　存放记录链表，以待处理(这里多采用多线程方式输出到文件)
**************************************************************************************/
RRecordList::RRecordList(void)
{

}

RRecordList& 
RRecordList::Instance(void)
{
    static RRecordList _instance;
    return _instance;
}

RRecordList::~RRecordList(void)
{
    SaveAllToFile();
}

bool 
RRecordList::IsEmpty(void)
{
    XLOCK(m_Mutex);
    return m_Data.empty();
}

void 
RRecordList::Push_back(RRecord* pRecord)
{
    XLOCK(m_Mutex);
    m_Data.push_back(pRecord);
}

void 
RRecordList::Swap(std::list<RRecord*>& list)            //得到日志数据复本,清除当前链表数据
{
    XLOCK(m_Mutex);
    m_Data.swap(list);
}

void 
RRecordList::SaveAllToFile(void)                          //将所有记录链表中信息存入文件
{
    if ( m_Data.size() == 0 )
        return;
    std::list<RRecord*> tmp;
    Swap(tmp);
    std::list<RRecord*>::iterator iter;
    for ( iter = tmp.begin(); iter != tmp.end(); iter++ )
    {
        RRecord* pRecord = *iter;
        pRecord->SaveToFile();
        delete pRecord;
    }
}
