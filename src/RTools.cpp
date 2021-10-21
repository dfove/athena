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

#include <RTools.hpp>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/timeb.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <regex.h>

namespace athena
{

#ifndef _WIN32
//设置信号处理器
SigFunc* Signal(int signo, SigFunc* func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (sigaction(signo, &act, &oact) < 0)
	{
		return 0;
	}
		
	return oact.sa_handler;
}
#endif

//获取当前日期YYYYMMDD
RString GetCurrDate(int nOffset)
{
	struct tm m;
	time_t t;
	char pBuf[9];

	t = time(NULL);
	t += nOffset*24*60*60;
#ifdef _WIN32
    localtime_s(&m, &t);
#else
	localtime_r(&t, &m);
#endif
	snprintf(pBuf, sizeof(pBuf), "%4d%02d%02d", m.tm_year+1900, m.tm_mon+1, m.tm_mday);

	return pBuf;
}
//获取当前时间HHMMSS
RString GetCurrTime(int nOffset)
{
	struct tm m;
	time_t t;
    char szBuf[7];

	time(&t);
	t += nOffset*24*60*60;
#ifdef _WIN32
    localtime_s(&m, &t);
#else
	localtime_r(&t, &m);
#endif
	snprintf(szBuf, sizeof(szBuf), "%02d%02d%02d", m.tm_hour,  m.tm_min, m.tm_sec);

	return szBuf;
}

//获取当前时间YYYYMMDDHHMMSS
RString GetCurrDateTime(int nOffset)
{
	struct tm m;
	time_t t;
    char szBuf[15];

	time(&t);
	t += nOffset*24*60*60;
#ifdef _WIN32
    localtime_s(&m, &t);
#else
	localtime_r(&t, &m);
#endif
	snprintf(szBuf, sizeof(szBuf), "%4d%02d%02d%02d%02d%02d", m.tm_year+1900, m.tm_mon+1, m.tm_mday,
		m.tm_hour,  m.tm_min, m.tm_sec);

	return szBuf;
}

//获取当前小时
int GetCurrHour()
{
	struct tm m;
	time_t t;

	t = time(NULL);
#ifdef _WIN32
    localtime_s(&m, &t);
#else
	localtime_r(&t, &m);
#endif
    return m.tm_hour;
}

//获取当前秒数
int GetCurrSec(void)
{
	struct tm m;
	time_t t;

	t = time(NULL);
#ifdef _WIN32
    localtime_s(&m, &t);
#else
	localtime_r(&t, &m);
#endif
    return ((m.tm_hour*60)+m.tm_min)*60+m.tm_sec;
}

RString GetLogDateTime()
{
	time_t t;
	struct tm m;
	struct timeb m1;
    char pBuf[32];

	t = time(NULL);
#ifdef _WIN32
    localtime_s(&m, &t);
#else
	localtime_r(&t, &m);
#endif
	ftime(&m1);
	snprintf(pBuf, sizeof(pBuf), "[%04d-%02d-%02d %02d:%02d:%02d.%03d]",m.tm_year + 1900, m.tm_mon + 1, m.tm_mday, m.tm_hour, m.tm_min, m.tm_sec,m1.millitm);	
    return pBuf;
}

RString GetDelimitCurrDate(char cFmt, int nOffset)
{
	struct tm m;
	time_t t;
    char pBuf[11];

	t = time(NULL);
	t += nOffset*24*60*60;
#ifdef _WIN32
    localtime_s(&m, &t);
#else
	localtime_r(&t, &m);
#endif
	snprintf(pBuf, sizeof(pBuf), "%4d%c%02d%c%02d", m.tm_year+1900, cFmt, m.tm_mon+1, cFmt, m.tm_mday);
	
	return pBuf;
}

RString GetDelimitCurrDateTime(char cFmt, int nOffset)
{
	struct tm m;
	time_t t;
	char pBuf[20];

	time(&t);
	t += nOffset*24*60*60;
#ifdef _WIN32
    localtime_s(&m, &t);
#else
	localtime_r(&t, &m);
#endif
	snprintf(pBuf, sizeof(pBuf), "%4d%c%02d%c%02d%c%02d%c%02d%c%02d", m.tm_year+1900, cFmt,m.tm_mon+1, cFmt, m.tm_mday,
		cFmt, m.tm_hour,  cFmt, m.tm_min, cFmt, m.tm_sec);
	
	return pBuf;
}

//是否超时
inline bool IsTimeOut(const time_t &tLast, int nCount)
{
	return (time(NULL) - tLast) >= nCount;
}

RString GetExeFilePath(void)
{
#ifdef _WIN32
        char exename[MAXPATHSIZE];
        GetModuleFileName(NULL,exename,MAXPATHSIZE);
        return exename;
#elif _LINUX_
        char exename[MAXPATHSIZE];
        int  count;
        count = readlink( "/proc/self/exe", exename, MAXPATHSIZE );
        if ( count < 0 || count >= 256 )
        {
            return RString("");
        }
        exename[ count ] = '\0';
        return exename;
#elif _SUNOS_
    RString sParam = getexecname();/*此函数仅在solaris上有效,得到execve的第一形参（比argv[0]可靠）*/
    return sParam;
#endif 
}

RString GetCurDir(void)
{
    char buf[MAXPATHSIZE];
    getcwd(buf,MAXPATHSIZE);
    return RString(buf) + PATH_SEP;
}

RString GetParentDirPath(const RString& sFilePath)                       //从带路径的文件名或目录名得到其父目录的路径
{
    RASSERT(sFilePath.size() > 0);
    RString sName(sFilePath);
    if(*(sName.rbegin()) == PATH_SEP)
    {//去掉目录的最后一个目录分隔符
        sName.resize(sName.size() - 1);
    }

    size_t nPos = sName.find_last_of(PATH_SEP);
    if(nPos == RString::npos)
    {
        return RString("");
    }

    return sName.substr(0,nPos+1);
}

RString GetNameFromFullPath(const RString& sFilePath)                    //从带路径的文件名或目录名获取其名称
{
    RASSERT(sFilePath.size() > 0);
    RString sName(sFilePath);
    if(*(sName.rbegin()) == PATH_SEP)
    {
        sName.resize(sName.size() - 1);
    }

    size_t nPos = sName.find_last_of(PATH_SEP);
    if(nPos == RString::npos)
    {
        return sName;
    }

    return sName.substr(nPos + 1);    
}

void Asc2Bcd(const char* asc, int ascLen, char* bcd, int bcdLen)
{
	memset(bcd, 0xFF, bcdLen);
	int len = ascLen < bcdLen*2?ascLen:bcdLen*2;    //the number of the char should translate to bcd

    for(int i = 0; i < len; i++)
    {
        if ( i%2==0 )
            bcd[i/2] &= ((asc[i]-'0')|0xF0);
        else
            bcd[i/2] &= ((asc[i]-'0')<<4|0x0F);
    }
}

void Bcd2Asc(const char* bcd, int bcdLen, char* asc, int ascLen)
{
    int len = 2*bcdLen < ascLen?2*bcdLen:ascLen;    //the number of the char translated to asc
    char* p = asc;
    for(int i=0; i < len; i++)
    {
        char b=*(bcd+i/2);
        if ( i%2 == 0 )
            *p++ = ((b&0x0f)+'0');
        else
            *p++ = (((b>>4)&0x0f)+'0');
    }
}

void PrintHex(const void* buf, int size)
{
    const unsigned char* pbuf = (const unsigned char*)buf;
    for (int cnt = 0; cnt < size; ++cnt)
        printf("%02x ",*(unsigned char*)(pbuf + cnt));
    printf("\n");
}

RString operator^(const RString& sDir,const RString& sFile)
{
    if(sDir.empty() || *(sDir.rbegin()) != PATH_SEP)
        return sDir + PATH_SEP + sFile;
    else
        return sDir + sFile;
}

bool cmp_reg_exp( const char* pattern, const char* str )
{
    int    status;
    regex_t    re;

    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) 
    {
      return false;      /* Report error. */
    }
    status = regexec(&re, str, (size_t) 0, NULL, 0);
    regfree(&re);
    if (status != 0)
    {
        return false;      /* Report error. */
    }
    return true;
}

}   //namespace
