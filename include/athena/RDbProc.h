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
 *  ����Ϊ�ṹSQLCA�Ķ���,�������ļ������ٰ���sqlca.h(SQLCA.H)Ҳ������ʹ��EXEC INCLUDE SQLCA��
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
     *                dbnameҲ��ΪdbLinkName
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

protected:  //�ṩ�������Ĭ�ϵ���������������д
    /*
     *  params:
     *      retryTimes ���Դ���
     *      retryInterval ÿ������֮��ļ��(��)
     */
    virtual bool ReConnect(int retryTimes = 6, int retryInterval = 10);

    /*
     *  ��ʹ��RDbProc��ʱ��ָ�������ʱ����ô˺���:
     *      EXEC SQL WHENEVER SQLERROR DO Sqlerror();
     *  �˺����ṩ�ڶϿ�����µļ򵥵���������(����6�Σ�ÿ�μ��10��)
     */
    virtual void Sqlerror();

protected:
    RString m_userName;

    RString m_password;

    RString m_dbName;

    RString m_dbLinkName;

    bool    m_isConnected;

    bool    m_autoReConn;

    SQLCA   sqlca;        /*ע��ֻ�ܶ����sqlca,�����޸Ĵ˱�����*/

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
