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

#ifndef RTIME_HPP_HEADER_INCLUDED
#define RTIME_HPP_HEADER_INCLUDED

#include <time.h>
#include <Unidef.hpp>
#include <RString.hpp>

namespace athena
{

typedef struct
{
    time_t          time;
    unsigned int    millitm;    //毫秒
    short           timezone;
    short           dstflag;
}
RTimeB;

typedef struct
{
  long tv_sec;
  long tv_nsec;
}
RTimeSpec;

typedef struct
{
    int tz_minuteswest; /* minutes W of Greenwich */
    int tz_dsttime;     /* type of dst correction */
}
RTimeZone;

typedef struct
{
    long tv_sec;        //
    long tv_usec;       //微秒
}
RTimeVal;

class ATHENA_EX RTime
{
public:
    static void Ftime( RTimeB * timeptr);

    static bool GetTimeOfDay( RTimeVal *tv, RTimeZone *tz = NULL);

    static time_t Time(time_t  *tmt);

    static char * Ctime(const time_t  *timep);

    static bool IsTimeout(time_t tLastTime, int nInterval);

    /*
     *  等待秒数
     */
    static void Sleep_s(int sec);

    /*
     *  等待毫秒数
     */
    static void Sleep_r(int milliseconds);

    /*
     *  等待微秒数
     */
    static void Sleep_u(int microsecond);

public:
	RTime(const time_t time = 0);

    /*
     *  真实的年(YYYY),月(1-12),日(1-31),时(0-23),分(0-59),秒(0-59)
     */
    RTime(const int nYear,const int nMonth,const int nDay,const int nHour,const int nMin,const int nSec);

    /*
     *  得到当前时间（将m_time设置为当前时间）
     */
    const RTime& GetCurrentTime(void);

    const RTime& PreDay(int nOffset = 1);

    const RTime& NextDay(int nOffset = 1);

    /*
     *  返回格式化后的时间字符串
     *  参数参见strftime的帮助
     *  "%Y" 2009   
     *  "%m" 12
     *  "%d" 01-31
     */
    RString Format(const char* lpszFormat) const;

    RString GetDate();  //YYYYMMDD

    RString GetTime();  //HHMMSS

    RString GetDateTime();  //YYYYMMDDHHMMSS

    /*
     *  得到tm格式的时间
     *  params:
     *      [OUT]t_tm 
     */
    void   LocalTime(struct tm& tm) const;

    /*
     *  从tm格式转为time_t格式，如果t_tm的数据不正确，将会被修正
     *  params:
     *      [IN/OUT]    t_tm
     */
    bool MakeTime(struct tm& tm);

    /*
     *  得到时间（返回m_time的值）
     */
    time_t GetTime_t(void) const;

    int GetYear(void) const;    //包含世纪

    int GetMonth(void) const;   //1-12

    int GetDay(void) const;     //1-31

    int GetHour(void) const;    //0-23

    int GetMinute(void) const;  //0-59

    int GetSecond(void) const;  //0-59

    int GetMillisecond(void) const; //0-999

    int GetDayOfWeek(void) const;   //0-6

    /*
     *  逝去多少秒（当前时间减去m_time的时间值）
     */
    int elapsed(void) const;

    /*
     *  赋值操作
     */
    RTime& operator=( const RTime& time );

    /*
     *  返回相差的秒数
     */
    int operator-(const RTime& time ) const;

    /*
     *  时间的比较,精确到秒
     */
    bool operator==(const RTime& time) const;

    bool operator!=(const RTime& time) const;

    bool operator<(const RTime& time) const;

    bool operator>(const RTime& time) const;

    bool operator<=(const RTime& time) const;

    bool operator>=(const RTime& time) const;

private:
    time_t  m_time;

    int     m_millisecond;

    struct tm   m_tm;
};

}

#endif /* RTIME_HPP_HEADER_INCLUDED */
