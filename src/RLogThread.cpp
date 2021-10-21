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

#include <RLogThread.hpp>
#include <RRecord.hpp>
#include <RTime.hpp>

using namespace athena;

RWriteLogThread::RWriteLogThread(unsigned sleepMicroSeconds /*= 1*/)
                :RThread("Record Thread"),
                 m_interval(sleepMicroSeconds)
{

}

RWriteLogThread::~RWriteLogThread(void)
{

}

void* 
RWriteLogThread::Exec(void)
{
	while(1)
    {
        SetCancelPoint();
        RRecordFileList& insFlist = RRecordFileList::Instance();
        RRecordList& ins = RRecordList::Instance();
        insFlist.CheckTimeToCloseFile();
        ins.SaveAllToFile();
        RTime::Sleep_u(m_interval);
    }
    return NULL;
}


RLogThread::RLogThread(const RString& lpszName, 
                       const RString& logPath, 
                       LOG_GRANULARITY iGranularity /* = PER_DAY */, 
                       bool bFlush /* = true */)
           :RThread(lpszName),
            m_log(lpszName, logPath, iGranularity, bFlush)
{

}

RLogThread::~RLogThread(void)
{

}

/*
void 
RLogThread::Fatal(const char* lpszFormat, ...) const
{
    va_list ap;
    va_start(ap, lpszFormat);
    m_log.FormatV(__FILE__, __LINE__, LOG_FATAL, lpszFormat, ap);
    va_end(ap);
}

void 
RLogThread::Error(const char* lpszFormat, ...) const
{
    va_list ap;
    va_start(ap, lpszFormat);
    m_log.FormatV(__FILE__, __LINE__, LOG_ERROR, lpszFormat, ap);
    va_end(ap);
}

void 
RLogThread::Warn(const char* lpszFormat, ...) const
{
    va_list ap;
    va_start(ap, lpszFormat);
    m_log.FormatV(__FILE__, __LINE__, LOG_WARN, lpszFormat, ap);
    va_end(ap);
}

void 
RLogThread::Info(const char* lpszFormat, ...) const
{
    va_list ap;
    va_start(ap, lpszFormat);
    m_log.FormatV(__FILE__, __LINE__, LOG_INFO, lpszFormat, ap);
    va_end(ap);
}

void 
RLogThread::Debug(const char* filename, int line, const char* lpszFormat, ...) const
{
    va_list ap;
    va_start(ap, lpszFormat);
    m_log.FormatV(filename, line, LOG_DEBUG, lpszFormat, ap);
    va_end(ap);
}
*/

void 
RLogThread::Log(const char *filename, int line, int nLevel, const char *lpszFormat, ...) const
{
    va_list ap;
    va_start(ap, lpszFormat);
    m_log.FormatV(filename, line, nLevel, lpszFormat, ap);
    va_end(ap);
}

void 
RLogThread::SetLogName(const RString& filename)
{
    m_log.SetName(filename);
}
