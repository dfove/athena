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

#include <RTxtParser.hpp>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>

using namespace athena;

RTxtParser::RTxtParser(const RString& filename)
           :m_sFileName(filename),
            m_valid(false)
{
    struct stat buf;
    if ( stat( m_sFileName.c_str(),  &buf) == 0 && S_ISREG(buf.st_mode) )
    {
        m_valid = true;
    }
}

RTxtParser::~RTxtParser()
{
    
}

void 
RTxtParser::ReadSection(const RString& sSection, std::list< RString > &strList)
{
	std::fstream fs(m_sFileName.c_str(), std::ios_base::in | std::ios_base::binary);
	RString line, sSub;
	bool bFound = false;

	if(!fs)
		return;

	while(!fs.eof())
	{
	    //fs >> line;
		getline(fs, line);
		Trim(line);
        if(IsShieldLine(line))
            continue;
		if(line.length() > 2)
		{
			//如果是section
			if(line[0] == '[' && line[line.length()-1] == ']')
			{
				sSub = line.substr(1, line.length()-2);
				bFound = (sSub == sSection);
				continue;   //读取section以下的内容
			}
			//如果找到相应的section，将section内的所有有效内容行读入列表
			if(bFound)
			{
				if(!IsShieldLine(line))
				{
					strList.push_back(line);
			    }
			}
		}
	}
}

RString 
RTxtParser::ReadString(const RString& sSection, const RString& sIdent, const RString& sDefault)
{
	std::list< RString > strList;
	int nPos;
	ReadSection(sSection, strList);
	std::list< RString >::iterator iter = strList.begin();
	for ( ; iter != strList.end(); iter++ )
	{
	    RString str = *iter;
		nPos = str.find(sIdent.c_str());
		if(nPos != -1)
		{
			return GetValue(str);
		}
	}
	return sDefault;
}

bool 
RTxtParser::ReadStringArray(const RString& sSection, const RString& sIdent, std::vector< RString >& vet)
{
    std::fstream fs(m_sFileName.c_str(), std::ios_base::in | std::ios_base::binary);
    if ( !fs )
        return false;
    RString line;
    bool bFound = false;
    while ( !fs.eof() )
    {
		getline(fs, line);
        Trim(line);
        if ( IsShieldLine(line) )
            continue;
		if(line.length() > 2)
		{
			if(line.at(0) == '[' && line.at(line.length()-1) == ']')
			{
				RString sSub = line.substr(1, line.length()-2);
				bFound = (sSub == sSection);
				continue;
			}
			if ( bFound )
			{
			    RString sSub = GetIdent(line);
			    Trim(sSub);
			    if ( sSub == sIdent )
			    {
			        RString sValue = GetValue(line);
			        Trim(sValue);
			        while ( 1 )
			        {
    			        size_t n = sValue.find_first_of(",");
    			        RString tmp = sValue.substr(0, n);
    			        Trim(tmp);
    			        if ( !tmp.empty() )
    			        {
        			        std::vector< RString >::iterator iter;
        			        iter = find(vet.begin(), vet.end(), tmp);
        			        if ( iter == vet.end() )
        			            vet.push_back(tmp);
    			        }
    			        if ( n == RString::npos )
    			        {
    			            break;
    			        }
                        sValue.erase(0,n+1);
			        }
			    }
			}
		}
    }
    return true;
}

int 
RTxtParser::ReadInteger(const RString& sSection, const RString& sIdent, int nDefault)
{
	RString s;

	s = ReadString(sSection, sIdent, "");
	if(s != "")
		return atoi(s.c_str());
	else
		return nDefault;
}

long 
RTxtParser::ReadLongInt(const RString sSection, const RString sIdent, long nDefault)
{
	RString s;

	s = ReadString(sSection, sIdent, "");
	if(s != "")
		return atol(s.c_str());
	else
		return nDefault;
}

bool 
RTxtParser::ReadBool(const RString& sSection, const RString& sIdent, bool bDefault)
{
	RString s;

	s = ReadString(sSection, sIdent, "");
    if ( s.empty() )
        return bDefault;
    transform(s.begin(),s.end(),s.begin(),std::ptr_fun<int,int>(tolower));   //全部字母变成小写,再比较
    if ( s == "true" || atoi(s.c_str()) != 0 )
        return true;
    return false;
}

float 
RTxtParser::ReadFloat(const RString& sSection, const RString& sIdent, float fDefault)
{
	RString s;

	s = ReadString(sSection, sIdent, "");
	if(s != "")
		return atof(s.c_str());
	else
		return fDefault;
}

bool 
RTxtParser::IsValid() const
{
    return m_valid;
}

bool 
RTxtParser::IsShieldLine(const RString& str)
{
	if( str.length() == 0 || str[0] == '#')
		return true;
	else
		return false;
}

void 
RTxtParser::Trim(RString& str)
{
	if ( str.length() > 0 && (str[str.length() - 1] == 0X0D || str[str.length() - 1] == 0X0A) )
	{
	    str.erase(str.length() - 1);
    }
	RString::iterator Iter;
	Iter = remove(str.begin(), str.end(), ' ');
	if(Iter != str.end())
	{
		str.erase(Iter, str.end());
	}
}

RString 
RTxtParser::GetIdent(const RString& str)
{
	RString Ident;
	int nPos;

	nPos = str.find("=");
	if(nPos != -1)
	{
		Ident = str.substr(0, nPos);
		Trim(Ident);
	}

	return Ident;
}

RString 
RTxtParser::GetValue(const RString& str)
{
	RString Value;
	RString::size_type nIndex;

	nIndex = str.find("=");
	if(nIndex != RString::npos)
	{
		Value = str.substr(nIndex+1, str.length()-nIndex);
		Trim(Value);
	}

	return Value;
}
