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

#ifndef RTXTPARSER_HPP_HEADER_INCLUDED
#define RTXTPARSER_HPP_HEADER_INCLUDED

#include <RString.hpp>
#include <list>
#include <vector>

namespace athena
{

class RTxtParser
{
public:
    RTxtParser(const RString& filename);

    virtual ~RTxtParser();

public:
	void ReadSection(const RString& sSection, std::list< RString > &List);

	RString ReadString(const RString& sSection, const RString& sIdent, const RString& sDefault = "");

    bool ReadStringArray(const RString& sSection, const RString& sIdent, std::vector< RString >& vet);

    int ReadInteger(const RString& sSection, const RString& sIdent, int nDefault);

    long ReadLongInt(const RString sSection, const RString sIdent, long nDefault);

	bool ReadBool(const RString& sSection, const RString& sIdent, bool bDefault);

	float ReadFloat(const RString& sSection, const RString& sIdent, float fDefault);

    bool IsValid() const;

private:
	bool IsShieldLine(const RString& str);

	void Trim(RString &str);

	RString GetIdent(const RString& str);

	RString GetValue(const RString& str);

private:
    RString m_sFileName;

    bool m_valid;
};

}   //namespace

#endif /*RTXTPARSER_HPP_HEADER_INCLUDED*/
