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

#include <RTime.hpp>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if defined (WIN32) || defined (_WINDOW)
#include <windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#endif
#include <sys/timeb.h>
#include <time.h>

using namespace athena;

const unsigned MAX_TIME_SIZE = 256;                //时间字符串最大长度值

void 
RTime::Ftime( RTimeB * timeptr)
{
#if defined (WIN32) || defined (_WINDOW)
    struct _timeb timebuffer;
    _ftime(&timebuffer);
    timeptr->millitm = timebuffer.millitm;
    timeptr->time = timebuffer.time;
    timeptr->timezone = timebuffer.timezone;
    timeptr->dstflag = timebuffer.dstflag;
#else
    struct timeb timebuffer;
    ::ftime(&timebuffer);
    timeptr->millitm = timebuffer.millitm;
    timeptr->time = timebuffer.time;
    timeptr->timezone = timebuffer.timezone;
    timeptr->dstflag = timebuffer.dstflag;
#endif
}

bool 
RTime::GetTimeOfDay( RTimeVal *tv, RTimeZone *tz)
{
#if defined (WIN32) || defined (_WINDOW)
    RTimeB timebuffer;
    RTime::Ftime(&timebuffer);
    tv->tv_sec = (long)timebuffer.time;
    tv->tv_usec = timebuffer.millitm * 1000;
#else
    struct timeval _tv;
    int i = gettimeofday(&_tv, NULL);
    if (i)
        return false;
    tv->tv_sec = _tv.tv_sec;
    tv->tv_usec = _tv.tv_usec;
#endif
    return true;
}

time_t 
RTime::Time(time_t  *tmt)
{
    RTimeB timebuffer;
    RTime::Ftime(&timebuffer);
    if ( tmt )
        *tmt = timebuffer.time;
    return timebuffer.time;
}

char *
RTime::Ctime(const time_t  *timep)
{
#ifndef _WIN32_WCE
    return ::ctime(timep);
#else  /* !_WIN32_WCE */
    return NULL;    //TODO
#endif
}

bool 
RTime::IsTimeout(time_t tLastTime, int nInterval)
{
    return time(NULL) - tLastTime > nInterval;
}

void 
RTime::Sleep_s(int sec)
{
    Sleep_u(sec*1000*1000);
}

void 
RTime::Sleep_r(int milliseconds)
{
    Sleep_u(milliseconds*1000);
}

void 
RTime::Sleep_u(int microseconds)
{
#if defined (WIN32) || defined (_WINDOWS)
    ::Sleep(microseconds/1000);
#else
    struct timespec req;
    struct timespec rem;
    memset(&rem, 0, sizeof(rem));
    req.tv_sec = microseconds/1000000;       
    req.tv_nsec = microseconds%1000000*1000;
    nanosleep(&req, &rem);
#endif
}

RTime::RTime(const time_t time /*= 0*/)
      :m_millisecond(0)
{
    m_time = time;
    LocalTime(m_tm);
}

RTime::RTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec)
      :m_time(0)
{
    memset(&m_tm, 0, sizeof(struct tm));
    m_tm.tm_sec = nSec;
    m_tm.tm_min = nMin;
    m_tm.tm_hour = nHour;
    RASSERT(nDay >= 1 && nDay <= 31);
    m_tm.tm_mday = nDay;
    RASSERT(nMonth >= 1 && nMonth <= 12);
    m_tm.tm_mon = nMonth - 1;        // tm_mon is 0 based
    RASSERT(nYear >= 1900);
    m_tm.tm_year = nYear - 1900;     // tm_year is 1900 based
    MakeTime(m_tm);
    RASSERT(m_time != (time_t)(-1));          // indicates an illegal input time
}

const RTime& 
RTime::GetCurrentTime(void)
{
    RTimeB timebuffer;
    RTime::Ftime(&timebuffer);
    m_time = timebuffer.time;
    m_millisecond = timebuffer.millitm;
    LocalTime(m_tm);
    return *this;
}

const RTime& 
RTime::PreDay(int nOffset /*= 1*/)
{
    m_time -= nOffset*24*60*60;
    LocalTime(m_tm);
    return *this;
}

const RTime& 
RTime::NextDay(int nOffset /*= 1*/)
{
    m_time += nOffset*24*60*60;
    LocalTime(m_tm);
    return *this;
}

RString 
RTime::Format(const char* lpszFormat) const
{
    RASSERT(lpszFormat != NULL && strlen(lpszFormat) < MAX_TIME_SIZE);
    char pszTime[MAX_TIME_SIZE];
    strftime(pszTime,sizeof(pszTime) - 1, lpszFormat,&m_tm);
    return RString(pszTime);
}

RString 
RTime::GetDate()
{
    char pBuf[9];
	snprintf(pBuf, sizeof(pBuf), "%4d%02d%02d", m_tm.tm_year+1900, m_tm.tm_mon+1, m_tm.tm_mday);
	return pBuf;
}

RString 
RTime::GetTime()
{
    char szBuf[7];
	snprintf(szBuf, sizeof(szBuf), "%02d%02d%02d", m_tm.tm_hour,  m_tm.tm_min, m_tm.tm_sec);
	return szBuf;
}

RString 
RTime::GetDateTime()
{
    char szBuf[15];
	snprintf(szBuf, sizeof(szBuf), "%4d%02d%02d%02d%02d%02d", m_tm.tm_year+1900, m_tm.tm_mon+1, m_tm.tm_mday,
		m_tm.tm_hour,  m_tm.tm_min, m_tm.tm_sec);
	return szBuf;
}

void  
RTime::LocalTime(struct tm& tm) const
{
#ifdef _WIN32
    localtime_s(&tm, &m_time);
#else
	localtime_r(&m_time, &tm);
#endif
}

bool 
RTime::MakeTime(struct tm& tm)
{
    m_time = mktime(&tm);
    m_tm = tm;
    return m_time != (time_t)(-1);
}

time_t 
RTime::GetTime_t(void) const
{
    return m_time;
}

int 
RTime::GetYear(void) const
{
    return m_tm.tm_year + 1900;
}

int 
RTime::GetMonth(void) const
{
    return m_tm.tm_mon + 1;
}

int 
RTime::GetDay(void ) const                     //1-31
{
    return m_tm.tm_mday;
}
int 
RTime::GetHour(void) const
{
    return m_tm.tm_hour;
}

int 
RTime::GetMinute(void) const
{
    return m_tm.tm_min;
}

int 
RTime::GetSecond(void) const
{
    return m_tm.tm_sec;
}

int 
RTime::GetMillisecond(void) const
{
    return m_millisecond;
}

int 
RTime::GetDayOfWeek(void) const
{
    return m_tm.tm_wday;
}

int 
RTime::elapsed(void) const                    //逝去多少秒
{
    RTime t;
    t.GetCurrentTime();
    return t - *this;
}

RTime& 
RTime::operator=( const RTime& time )  
{
    m_time = time.m_time;
    m_millisecond = time.m_millisecond;
    LocalTime(m_tm);
    return( *this );
}

int 
RTime::operator-(const RTime& time )  const
{
    return static_cast<int>(m_time - time.m_time);
}

bool 
RTime::operator==(const RTime& time ) const
{
    return m_time == time.m_time;
}

bool 
RTime::operator!=(const RTime& time ) const
{
    return( m_time != time.m_time );
}

bool 
RTime::operator<(const RTime& time ) const
{
    return( m_time < time.m_time );
}

bool 
RTime::operator>(const RTime& time ) const
{
    return( m_time > time.m_time );
}

bool 
RTime::operator<=(const RTime& time ) const
{
    return( m_time <= time.m_time );
}

bool 
RTime::operator>=(const RTime& time ) const
{
    return( m_time >= time.m_time );
}
