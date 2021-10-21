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
    unsigned int    millitm;    //����
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
    long tv_usec;       //΢��
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
     *  �ȴ�����
     */
    static void Sleep_s(int sec);

    /*
     *  �ȴ�������
     */
    static void Sleep_r(int milliseconds);

    /*
     *  �ȴ�΢����
     */
    static void Sleep_u(int microsecond);

public:
	RTime(const time_t time = 0);

    /*
     *  ��ʵ����(YYYY),��(1-12),��(1-31),ʱ(0-23),��(0-59),��(0-59)
     */
    RTime(const int nYear,const int nMonth,const int nDay,const int nHour,const int nMin,const int nSec);

    /*
     *  �õ���ǰʱ�䣨��m_time����Ϊ��ǰʱ�䣩
     */
    const RTime& GetCurrentTime(void);

    const RTime& PreDay(int nOffset = 1);

    const RTime& NextDay(int nOffset = 1);

    /*
     *  ���ظ�ʽ�����ʱ���ַ���
     *  �����μ�strftime�İ���
     *  "%Y" 2009   
     *  "%m" 12
     *  "%d" 01-31
     */
    RString Format(const char* lpszFormat) const;

    RString GetDate();  //YYYYMMDD

    RString GetTime();  //HHMMSS

    RString GetDateTime();  //YYYYMMDDHHMMSS

    /*
     *  �õ�tm��ʽ��ʱ��
     *  params:
     *      [OUT]t_tm 
     */
    void   LocalTime(struct tm& tm) const;

    /*
     *  ��tm��ʽתΪtime_t��ʽ�����t_tm�����ݲ���ȷ�����ᱻ����
     *  params:
     *      [IN/OUT]    t_tm
     */
    bool MakeTime(struct tm& tm);

    /*
     *  �õ�ʱ�䣨����m_time��ֵ��
     */
    time_t GetTime_t(void) const;

    int GetYear(void) const;    //��������

    int GetMonth(void) const;   //1-12

    int GetDay(void) const;     //1-31

    int GetHour(void) const;    //0-23

    int GetMinute(void) const;  //0-59

    int GetSecond(void) const;  //0-59

    int GetMillisecond(void) const; //0-999

    int GetDayOfWeek(void) const;   //0-6

    /*
     *  ��ȥ�����루��ǰʱ���ȥm_time��ʱ��ֵ��
     */
    int elapsed(void) const;

    /*
     *  ��ֵ����
     */
    RTime& operator=( const RTime& time );

    /*
     *  ������������
     */
    int operator-(const RTime& time ) const;

    /*
     *  ʱ��ıȽ�,��ȷ����
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
