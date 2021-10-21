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
	 *  异常类
	 *  params:
	 *		excpmsg      异常信息
	 *		syserrno	系统errno值
	 *      file        异常所在文件，通常为异常所在文件的__FILE__
	 *      line        异常所在文件行数，通常为异常所在文件的__LINE__
	 *      lpszMsg     用户自定义异常信息,而不是使用系统errno获取
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
