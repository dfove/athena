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

#ifndef RSTRING_HPP_HEADER_INCLUDED
#define RSTRING_HPP_HEADER_INCLUDED

#include <Unidef.hpp>

#include <stdarg.h>
#include <limits.h>
#include <string>
#include <vector>

namespace athena
{

class ATHENA_EX RString : public std::string
{
public:
    RString();

    /*
     *  str不允许为空指针
     */
    RString(const char* str);

    RString(const std::string& str);

    RString(const RString& str);

    RString(const char* str, const size_type count);

    RString(const RString& str, const size_type start,const size_type count);

    virtual ~RString();

/*operators*/
public:
    reference operator[](size_type _Off)
    {
        return std::string::operator [](_Off);
    }

    reference operator[](int _Off)
    {
        return std::string::operator [](_Off);
    }

    operator const char*() const
    {
        return c_str();
    }

    RString& operator=(const char * str);

    RString& operator=(const RString & str);

public:
    void FormatV(const char *formatStr, va_list argList);

    const RString& Format(const char *formatStr, ...);

    const RString& TrimLeft(const char* str = NULL);

    const RString& TrimRight(const char* str = NULL);

    const RString& TrimBothSide(const char* str = NULL);

    const RString& TrimLeftSpaces();

    const RString& TrimRightSpaces();

    const RString& TrimBothSideSpaces();

    const RString& TrimLeftLWS();

    const RString& TrimRightLWS();

    const RString& TrimBothSideLWS();

    const RString& MakeUpper(void);

    const RString& MakeLower(void);

    /*
     *  得到以lpszDelimit分隔的多个单词(用以替代strtok)
     */
    std::vector<RString> Strtok(const char *lpszDelimit) const;
public:
    /*
     *  < 0 less than str
     *  0   identical to str 
     *  > 0 greater than str 
     * params:
     *      count = -1, to compare all words of the string1 and string2
     */
    int Compare(const char* str, int count = -1) const;

    int Compare(const RString& str, int count = -1) const;

    /*
     *  compare two strings ignoring case
     */
    int CompareNoCase(const char* str, int count = -1) const;

    int CompareNoCase(const RString& str, int count = -1) const;

    bool IsDigit() const;

    bool IsValidIPAddress() const;
/*

public:
    BOOL RegMatch(const char* pattern) const;                   //正则表达式的匹配

    BOOL Match(const char* sMatch) const;                       //带*?的匹配

	vector<RString> RegMatchEx(const RString& strPattern, int nSize=10, int cflags = -1);//正则表达式的匹配,返回匹配后每个字段的值
*/
};

}       /*namespace athena*/

#endif /*RSTRING_HPP_HEADER_INCLUDED*/
