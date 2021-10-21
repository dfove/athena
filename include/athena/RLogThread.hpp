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

#ifndef RLOGTHREAD_HPP_HEADER_INCLUDED
#define RLOGTHREAD_HPP_HEADER_INCLUDED

#include <RLog.hpp>
#include <RThread.hpp>

namespace athena
{

class ATHENA_EX RWriteLogThread : public RThread
{
public:
    RWriteLogThread(unsigned sleepMicroSeconds = 1);

    virtual ~RWriteLogThread(void);

    virtual void* Exec(void);

private:
    unsigned m_interval; //线程运行停顿时间(微秒)
};


class ATHENA_EX RLogThread : public RThread
{
public:
    RLogThread(const RString& lpszName, const RString& logPath, LOG_GRANULARITY iGranularity = PER_DAY, bool bFlush = true);

    virtual ~RLogThread(void);

/*
public:
    void Fatal(const char* lpszFormat, ...) const;

    void Error(const char* lpszFormat, ...) const;

    void Warn(const char* lpszFormat, ...) const;

    void Info(const char* lpszFormat, ...) const;

    void Debug(const char* filename, int line, const char* lpszFormat, ...) const;
*/

/*
 *  日志功能函数
 *      Log(FATAL, "this is the log:%d", i);
 *      Log(ERROR, "this is the log:%d", i);
 *      Log(WARN, "this is the log:%d", i);
 *      Log(INFO, "this is the log:%d", i);
 *      Log(DEBUG, "this is the log:%d", i);
 */
public:
    void Log(const char *filename, int line, int nLevel, const char *lpszFormat, ...) const;

    void SetLogName(const RString& filename);

private:
    RLog    m_log;
};

}   //namespace

#endif /*RLOGTHREAD_HPP_HEADER_INCLUDED*/
