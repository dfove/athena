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

#include <RString.hpp>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <functional>
#include <algorithm>

using namespace athena;

RString::RString()
        :std::string()
{
}

RString::RString(const char* str)
        :std::string(str==NULL?"":str)
{
}

RString::RString(const std::string& str)
        :std::string(str)
{
}

RString::RString(const RString& str)
        :std::string(str.c_str())
{
    
}

RString::RString(const char* str, const size_type count)
        :std::string(str, count)
{
    
}

RString::RString(const RString& str, const size_type start,const size_type count)
        :std::string(str,start,count)
{
    
}

RString::~RString()
{
    
}

RString& 
RString::operator=(const char* str)
{
    if ( str != NULL )
    {
        clear();
        assign(str);
    }
    return *this;
}

RString& 
RString::operator=(const RString & str)
{
    if ( &str == this )
        return *this;
    clear();
    assign(str.c_str());
    return *this;
}


//#if ( defined(WIN32) || defined(_WINDOWS) )
//#define VSNPRINTF _vsnprintf
//#else
//#define VSNPRINTF vsnprintf
//#endif
void 
RString::FormatV(const char *formatStr, va_list argList)
{
    int n, size = 256;
    char *p = NULL, *np = NULL;
    if ((p = (char *)malloc (size)) == NULL)
        return;

    while (true) 
    {
		#if ( defined(WIN32) || defined(_WINDOWS) )
			n = _vsnprintf_s(p, size, size, formatStr, argList);
		#else
			n = vsnprintf(p, size, formatStr, argList);
		#endif
        if (n > -1 && n < size)
        {
           assign(p);
           break;
        }
        /* Else try again with more space. */
        if (n > -1)    /* glibc 2.1 */
        {
           size = n+1; /* precisely what is needed */
        }
        else           /* glibc 2.0 */
        {
           size *= 2;  /* twice the old size */
        }
        if ( (np = (char *)realloc (p, size)) == NULL ) 
        {
           free(p);
        }
        else 
        {
           p = np;
        }
    }
    if ( p )
    {
        free(p);
    }
}

const RString& 
RString::Format(const char *formatStr, ...)
{
    if ( formatStr == NULL )
        return *this;
    va_list ap;
    va_start(ap, formatStr);
    FormatV(formatStr, ap);
    va_end(ap);
    return *this;
}

const RString& 
RString::TrimLeft(const char* str)
{
    if ( str && !empty() )
        erase(0,find_first_not_of(str));
    return *this;
}

const RString& 
RString::TrimRight(const char* str)
{
    if ( str && !empty() )
        erase(find_last_not_of(str)+1);
    return *this;
}

const RString& 
RString::TrimBothSide(const char* str)
{
    TrimLeft(str);
    TrimRight(str);
    return *this;
}

const RString& 
RString::TrimLeftSpaces()
{
    TrimLeft("\t ");
    return *this;
}

const RString& 
RString::TrimRightSpaces()
{
    TrimRight("\t ");
    return *this;
}

const RString& 
RString::TrimBothSideSpaces()
{
    TrimLeftSpaces();
    TrimRightSpaces();
    return *this;
}

const RString& 
RString::TrimLeftLWS()
{
    TrimLeft("\r\n \t");
    return *this;
}

const RString& 
RString::TrimRightLWS()
{
    TrimRight("\r\n \t");
    return *this;
}

const RString& 
RString::TrimBothSideLWS()
{
    TrimLeftLWS();
    TrimRightLWS();
    return *this;
}

const RString& 
RString::MakeUpper(void)
{
    transform(begin(),end(),begin(),std::ptr_fun<int,int>(toupper));
    return *this;
}

const RString& 
RString::MakeLower(void)
{
    transform(begin(),end(),begin(),std::ptr_fun<int,int>(tolower));
    return *this;
}

std::vector<RString> 
RString::Strtok(const char *lpszDelimit) const
{
    std::vector<RString> vRe;
    if ( lpszDelimit == NULL )
        return vRe;
    RString sDelimit(lpszDelimit);
    if(sDelimit.empty())
    {
        vRe.push_back(*this);       //如果分隔符为空则返回当前字符串
        return vRe;
    }
    
    RString tmp(*this);
    while(1)
    {
        tmp.TrimBothSide(lpszDelimit);
        size_t nPos = tmp.find_first_of(lpszDelimit);
        vRe.push_back(tmp.substr(0,nPos));
        if(nPos == npos)
            break;
        tmp.erase(0,nPos);
    }
    return vRe;
}

int 
RString::Compare(const char* str, int count) const
{
    if ( count == -1 )
        return strcmp(c_str(), str);
    return strncmp(c_str(), str, count);
}

int 
RString::Compare(const RString& str, int count) const
{
    if ( count == -1 )
        return strcmp(c_str(), str.c_str());
    return strncmp(c_str(), str.c_str(), count);
}

/*
 *  for compare two strings ignoring case
 */
#if ( defined(WIN32) || defined(_WINDOWS) )
#define COMPARE_NO_CASE _stricmp
#define COMPARE_N_NO_CASE _strnicmp
#else
#define COMPARE_NO_CASE strcasecmp
#define COMPARE_N_NO_CASE strncasecmp
#endif

int 
RString::CompareNoCase(const char* str, int count) const
{
    if ( count == -1 )
        return COMPARE_NO_CASE(c_str(), str);
    return COMPARE_N_NO_CASE(c_str(), str, count);
}

int 
RString::CompareNoCase(const RString& str, int count) const
{
    if ( count == -1 )
        return COMPARE_NO_CASE(c_str(), str.c_str());
    return COMPARE_N_NO_CASE(c_str(), str, count);
}

bool 
RString::IsDigit() const
{
    return (end() == find_if(begin(),end(),not1(std::ptr_fun<int,int>(isdigit))));
}

bool 
RString::IsValidIPAddress() const
{
    const char* str = c_str();
    if ( !str || str[0] == '.' || str[strlen(str) - 1] == '.' )
        return false;

    int ndot = 0, nNumLen = 0;
    size_type len = npos, npos = 0;
    while (  ( len = find_first_of('.', npos) ) != npos )
    {
        ndot++;
        if ( len > 3 || len == 0 || ndot > 3 )
            return false;
        nNumLen = 0;
        for ( unsigned i = 0; i < len; i++ )
        {
            if ( *(str+npos+i) < '0' || *(str+npos+i) > '9')
            {
                return false;
            }
            if ( ++nNumLen > 3 ) 
            {
                return false;
            }
        }
        npos = len + 1;
    }
    return true;
}
