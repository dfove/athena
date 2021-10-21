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

#include <RLog.hpp>
#include <RTools.hpp>
#include <assert.h>

using namespace athena;

int RLog::m_loglevel = LOG_INFO;

RLog::RLog(LOG_GRANULARITY iGranularity /* = PER_DAY */)
     :RRecord("", "", "", "log", false, iGranularity==PER_DAY?86400:3600)
{
    
}

RLog::RLog(const RString& strName,
           const RString& strDir /* = "./" */,
           LOG_GRANULARITY iGranularity /* = PER_DAY */,
           bool bFlush /* = true */)
     :RRecord(strName, strDir, "", "log", false, iGranularity==PER_DAY?86400:3600, false, bFlush)
{

}

RLog::RLog(const RLog& right)
     :RRecord(right)
{
    
}

RLog::~RLog()
{

}

RRecord* 
RLog::Clone() const
{
    return new RLog(*this);
}

/*
void 
RLog::Fatal(const char* lpszFormat, ...) const
{
    va_list ap;
    va_start(ap, lpszFormat);
    FormatV(__FILE__, __LINE__, LOG_FATAL, lpszFormat, ap);
    va_end(ap);
}

void 
RLog::Error(const char* lpszFormat, ...) const
{
    va_list ap;
    va_start(ap, lpszFormat);
    FormatV(__FILE__, __LINE__, LOG_ERROR, lpszFormat, ap);
    va_end(ap);
}

void 
RLog::Warn(const char* lpszFormat, ...) const
{
    va_list ap;
    va_start(ap, lpszFormat);
    FormatV(__FILE__, __LINE__, LOG_WARN, lpszFormat, ap);
    va_end(ap);
}

void 
RLog::Info(const char* lpszFormat, ...) const
{
    va_list ap;
    va_start(ap, lpszFormat);
    FormatV(__FILE__, __LINE__, LOG_INFO, lpszFormat, ap);
    va_end(ap);
}

void 
RLog::Debug(const char* filename, int line, const char* lpszFormat, ...) const
{
    va_list ap;
    va_start(ap, lpszFormat);
    FormatV(filename, line, LOG_DEBUG, lpszFormat, ap);
    va_end(ap);
}
*/

void 
RLog::Log(const char *filename, int line, int nLevel, const char *lpszFormat, ...) const
{
    va_list ap;
    va_start(ap, lpszFormat);
    FormatV(filename, line, nLevel, lpszFormat, ap);
    va_end(ap);
}

void 
RLog::Write(const char *filename, int line, int nLevel, const char *lpszFormat, ...) const
{
    va_list ap;
    va_start(ap, lpszFormat);
    FormatV(filename, line, nLevel, lpszFormat, ap);
    va_end(ap);
}

void 
RLog::FormatV(const char *filename, int line, int nLevel, const char *lpszFormat, va_list argList) const
{
    if ( nLevel > RLog::m_loglevel )
        return;
    RASSERT(lpszFormat != NULL);

    //记录时间
    RTime timeNow(time(0));
    RString strDate;
    strDate.Format("[%s:%02d:%02d:%02d]", timeNow.GetDate().c_str(), timeNow.GetHour(), timeNow.GetMinute(), timeNow.GetSecond());

    //记录内容
    RString strBuf;
    strBuf.FormatV(lpszFormat, argList);
    RRecord* rec = Clone();
    rec->SetTime(timeNow.GetTime_t());

    if ( nLevel == LOG_DEBUG || m_loglevel == LOG_DEBUG )
    {
        RASSERT(filename != NULL);
        rec->FormatTimeStr(false, "%s %s:[%s:%d]%s", strDate.c_str(), LevelInfo[nLevel], GetNameFromFullPath(filename).c_str(), line, strBuf.c_str());
    }
    else
    {
        rec->FormatTimeStr(false, "%s %s:%s", strDate.c_str(), LevelInfo[nLevel], strBuf.c_str());
    }

    PushToList(rec);
    return;
}
