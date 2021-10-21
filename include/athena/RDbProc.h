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

#ifndef RDBPROC_HPP_HEADER_INCLUDED
#define RDBPROC_HPP_HEADER_INCLUDED

#include <Unidef.hpp>
#include <RString.hpp>

namespace athena
{

#define CONNECTION_IS_BROKEN(sqlca) (sqlca.sqlcode == -12571 || \
                                     sqlca.sqlcode == -1089 || \
                                     sqlca.sqlcode == -1090 || \
                                     sqlca.sqlcode == -3114 || \
                                     sqlca.sqlcode == -3113 || \
                                     sqlca.sqlcode == -1092 || \
                                     sqlca.sqlcode == -12326 || \
                                     sqlca.sqlcode == -12329 || \
                                     sqlca.sqlcode == -1012)
#define DB_DISCONNECT   -1012

/*
 *  以下为结构SQLCA的定义,包含此文件后不能再包含sqlca.h(SQLCA.H)也不能再使用EXEC INCLUDE SQLCA等
 */
struct SQLCA
{
    char    sqlcaid[8];
    int     sqlabc;
    int     sqlcode;
    struct
    {
        unsigned short sqlerrml;
        char           sqlerrmc[70];
    } sqlerrm;
    char    sqlerrp[8];
    int     sqlerrd[6];
    char    sqlwarn[8];
    char    sqlext[8];
};

class ATHENA_EX RDbProc
{
public:
    RDbProc(bool autoReconn = true);

    RDbProc(const RString& strDbConn, bool autoReconn /*= true*/);

    RDbProc(const RString& username, 
            const RString& passwd, 
            const RString& dbName,
            bool autoReconn = true);

    RDbProc(const RString& username, 
            const RString& passwd, 
            const RString& dbName, 
            const RString& dbLink,
            bool autoReconn = true);

    virtual ~RDbProc();

    virtual bool Connect();

    /*
     *  params:
     *      strDbConn usrname/password@dbname
     *                dbname也作为dbLinkName
     */
    virtual bool Connect(const RString& strDbConn, bool is);

    virtual bool Connect(const RString& username,
                         const RString& passwd,
                         const RString& dbName);

    virtual bool Connect(const RString& username,
                         const RString& passwd,
                         const RString& dbName,
                         const RString& dbLink);

    virtual void DisconDb();

    bool Commit();

    bool Rollback();

    RString  ReportErr(void);

protected:  //提供出错情况默认的重连操作，可重写
    /*
     *  params:
     *      retryTimes 重试次数
     *      retryInterval 每次重试之间的间隔(秒)
     */
    virtual bool ReConnect(int retryTimes = 6, int retryInterval = 10);

    /*
     *  在使用RDbProc的时候，指定出错的时候调用此函数:
     *      EXEC SQL WHENEVER SQLERROR DO Sqlerror();
     *  此函数提供在断开情况下的简单的重连操作(重连6次，每次间隔10秒)
     */
    virtual void Sqlerror();

protected:
    RString m_userName;

    RString m_password;

    RString m_dbName;

    RString m_dbLinkName;

    bool    m_isConnected;

    bool    m_autoReConn;

    SQLCA   sqlca;        /*注意只能定义成sqlca,不可修改此变量名*/

#ifdef SQL_CRSR
    EXEC SQL BEGIN DECLARE SECTION;
    sql_context m_sqlContext;
    EXEC SQL END DECLARE SECTION;
#else
    void*   m_sqlContext;
#endif

};

}   /*namespace*/

#endif /*RDBPROC_HPP_HEADER_INCLUDED*/
