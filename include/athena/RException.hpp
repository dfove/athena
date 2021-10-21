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

#ifndef REXCEPTION_HPP_HEADER_INCLUDED
#define REXCEPTION_HPP_HEADER_INCLUDED

#include <Unidef.hpp>
#include <RString.hpp>

namespace athena
{

class ATHENA_EX RException : public std::exception
{
public:
	/*
	 *  �쳣��
	 *  params:
	 *		excpmsg      �쳣��Ϣ
	 *		syserrno	ϵͳerrnoֵ
	 *      file        �쳣�����ļ���ͨ��Ϊ�쳣�����ļ���__FILE__
	 *      line        �쳣�����ļ�������ͨ��Ϊ�쳣�����ļ���__LINE__
	 *      lpszMsg     �û��Զ����쳣��Ϣ,������ʹ��ϵͳerrno��ȡ
	 */
	RException(const RString& excpmsg) throw();

	RException(int syserrno, const char* file = NULL, int line = -1) throw();

	RException(const char* lpszMsg, const char* file = NULL, int nLine = -1) throw();

 	virtual ~RException() throw();

	virtual const RString& What() const throw();

    virtual const char *what( ) const throw();

//private:
//    virtual const char *what( ) const throw( );

protected:
	RString     m_sMsg;

	int         m_nErrno;

	RString		m_fileName;

	int			m_lineNum;
};

}       /*namespace*/

#endif /* REXCEPTION_HPP_HEADER_INCLUDED */
