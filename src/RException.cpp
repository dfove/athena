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

#include <RException.hpp>
#include <errno.h>
#include <string.h>

namespace athena
{
#if defined (WIN32) || defined (_WINDOWS)
    #include <windows.h>

#else   //Linux or Unix
    
#endif
}

using namespace athena;

RException::RException(const RString& excpmsg) throw()
           :m_sMsg(excpmsg),
            m_nErrno(-1),
            m_lineNum(-1)
            
{

}

RException::RException(int syserrno, const char* file /*= NULL*/, int line /*= -1*/) throw()
           :m_nErrno(syserrno),
            m_lineNum(line)
{
    if ( file != NULL )
        m_fileName = file;
    if ( !m_fileName.empty() )
        m_sMsg += m_fileName;
    if ( m_lineNum != -1 )
    {
        RString tmp;
        if ( !m_sMsg.empty() )
            tmp.Format(", line %d", m_lineNum);
        else
            tmp.Format("line %d", m_lineNum);
        m_sMsg += tmp;
    }
    if ( m_nErrno != -1 )
    {
        RString tmp;
        char ctmp[256];
        char* strerr;
      #if defined (WIN32) || defined (_WINDOWS)
        strerror_s(ctmp, sizeof(ctmp), m_nErrno);
        strerr = ctmp;
      #else
        strerror_r(m_nErrno, ctmp, sizeof(ctmp));
        strerr = ctmp;
      #endif
        if ( !m_sMsg.empty() )
            tmp.Format(": errno %d, %s", m_nErrno, strerr);
        else
            tmp.Format("errno:%d, %s", m_nErrno, strerr);
        m_sMsg += tmp;
    }
}

RException::RException(const char* lpszMsg, const char* file /*= NULL*/, int nLine /*= -1*/) throw()
           :m_nErrno(-1),
            m_lineNum(nLine)
{
    if ( file != NULL )
        m_fileName = file;

    if ( !m_fileName.empty() )
    {
        m_sMsg = m_fileName;
	}
    if ( m_lineNum != -1 )
    {
        RString tmp;
        if ( m_sMsg.empty() )
            tmp.Format("line %d", m_lineNum);
        else
            tmp.Format(", line %d", m_lineNum);
        m_sMsg += tmp;
    }
    if ( lpszMsg != NULL )
    {
        RString tmp;
        if ( m_sMsg.empty() )
            tmp.Format("%s", lpszMsg);
        else
            tmp.Format(": %s", lpszMsg);
        m_sMsg += tmp;
    }
}

RException::~RException() throw()
{

}

const RString& 
RException::What() const throw()
{
    return m_sMsg;
}

const char*
RException::what( ) const throw()
{
    return m_sMsg.c_str();
}
