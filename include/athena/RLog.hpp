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

#ifndef RLOG_HPP_HEADER_INCLUDED
#define RLOG_HPP_HEADER_INCLUDED

#include <RRecord.hpp>

#include <stdarg.h>

namespace athena
{

typedef enum
{
    LOG_FATAL   = 0,    //严重错误
    LOG_ERROR   = 1,    //错误
    LOG_WARN    = 2,    //警告
    LOG_INFO    = 3,    //信息
    LOG_DEBUG   = 4     //调试
}LOG_LEVEL;

#define FATAL   __FILE__, __LINE__, LOG_FATAL
#define ERROR   __FILE__, __LINE__, LOG_ERROR
#define WARN    __FILE__, __LINE__, LOG_WARN
#define INFO    __FILE__, __LINE__, LOG_INFO
#define DEBUG   __FILE__, __LINE__, LOG_DEBUG

static const char LevelInfo[5][6]= 
{
    "FATAL", "ERROR", "WARN", "INFO", "DEBUG"
};

typedef enum
        {
            PER_DAY  =    0,               //每天一个日志文件
            PER_HOUR =    1                //每小时一个日志文件
        }
        LOG_GRANULARITY;

class ATHENA_EX RLog : public RRecord
{
public:
    RLog(LOG_GRANULARITY iGranularity = PER_DAY);    //默认每天一个日志文件

    RLog(const RString& strName,          //记录名称
         const RString& strDir = "./",    //目录
         LOG_GRANULARITY iGranularity = PER_DAY,         //粒度(秒),默认每天
         bool bFlush = true);

    RLog(const RLog& right);

    virtual ~RLog();

    virtual RRecord* Clone() const;

/*
 *  日志功能函数方式一
 *      log.Fatal("this is the log:%d", i);
 *      log.Debug(__FILE__, __LINE__, "this is the log:%d", i);
public:
    void Fatal(const char* lpszFormat, ...) const;

    void Error(const char* lpszFormat, ...) const;

    void Warn(const char* lpszFormat, ...) const;

    void Info(const char* lpszFormat, ...) const;

    void Debug(const char* filename, int line, const char* lpszFormat, ...) const;
 */

/*
 *  日志功能函数方式二
 *      log.Write(FATAL, "this is the log:%d", i);
 *      log.Write(ERROR, "this is the log:%d", i);
 *      log.Write(WARN, "this is the log:%d", i);
 *      log.Write(INFO, "this is the log:%d", i);
 *      log.Write(DEBUG, "this is the log:%d", i);
 *  Log是Write的别名，功能相同，采用哪个函数，取决于用户的使用习惯
 */
public:
    void Log(const char *filename, int line, int nLevel, const char *lpszFormat, ...) const;

    void Write(const char *filename, int line, int nLevel, const char *lpszFormat, ...) const;

    void FormatV(const char *filename, int line, int nLevel, const char *lpszFormat, va_list argList) const;

//日志级别
public:
    /*
     *  params:
     *      LOG_FATAL   = 0,    //严重错误
     *      LOG_ERROR   = 1,    //错误
     *      LOG_WARN    = 2,    //警告
     *      LOG_INFO    = 3,    //信息
     *      LOG_DEBUG   = 4     //调试
     */
    static inline void SetLogLevel(int loglevel = LOG_INFO)
    {
        m_loglevel = loglevel;
    }

    static inline int GetLogLevel() 
    {
        return m_loglevel;
    }

private:
    static int m_loglevel;  //日志等级，程序运行时的等级，仅有不大于这个等级的日志会被记录

};

template <class _Tp = RRecord>
class ATHENA_EX RRecordFactory
{
public:
    typedef _Tp value_type;

    RRecordFactory();

    virtual ~RRecordFactory();

    _Tp* GetLog() const;

private:
    _Tp* m_record;

};

template <class _Tp>
RRecordFactory< _Tp >::RRecordFactory()
{
    m_record = new _Tp;
}

template <class _Tp>
RRecordFactory< _Tp >::~RRecordFactory()
{
    if ( m_record != NULL )
    {
        delete m_record;
        m_record = NULL;
    }
}

template <class _Tp>
_Tp* 
RRecordFactory< _Tp >::GetLog() const
{
    return NULL;
}

#define RLogFactory RRecordFactory<RLog>

}   //namespace

#endif /*RLOG_HPP_HEADER_INCLUDED*/
