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

#ifndef RTOOLS_HPP_HEADER_INCLUDED
#define RTOOLS_HPP_HEADER_INCLUDED

#include <list>
#include <map>
#include <RString.hpp>
#include <Unidef.hpp>

#if defined (WIN32) || defined (_WINDOWS)
    #include <direct.h>
    #include <windows.h>
    const char PATH_SEP = '\\';
    #define access        _access
    #define getcwd        _getcwd
    #define snprintf      _snprintf
    #define mkdir(path,mode)        _mkdir(path)    //window下面的目录没有权限一说
#else
    const char PATH_SEP = '/';
#endif

namespace athena
{
#if defined (WIN32) || defined (_WINDOWS)

#else
typedef ATHENA_EX void SigFunc(int);

ATHENA_EX SigFunc* Signal(int signo, SigFunc* func);
#endif

ATHENA_EX RString GetCurrDate(int nOffset=0);

ATHENA_EX RString GetCurrTime(int nOffset=0);

ATHENA_EX RString GetCurrDateTime(int nOffset=0);

ATHENA_EX int GetCurrHour();

ATHENA_EX int GetCurrSec();

ATHENA_EX RString GetLogDateTime();

ATHENA_EX RString GetDelimitCurrDate(char cFmt, int nOffset=0);

ATHENA_EX RString GetDelimitCurrDateTime(char cFmt, int nOffset=0);

ATHENA_EX bool IsTimeOut(const time_t &tLast, int nCount);

ATHENA_EX RString GetExeFilePath(void);     //得到执行程序的带路径全名称

ATHENA_EX RString GetCurDir(void);          //得到当前目录名

ATHENA_EX RString GetParentDirPath(const RString& sFilePath);       //从带路径的文件名或目录名得到其父目录的路径

ATHENA_EX RString GetNameFromFullPath(const RString& sFilePath);    //从带路径的文件名或目录名获取其名称

ATHENA_EX void Asc2Bcd(const char* asc, int ascLen, char* bcd, int bcdLen);    //将123456 转换成 0X21 0X43 0X65

ATHENA_EX void Bcd2Asc(const char* bcd, int bcdLen, char* asc, int ascLen);    //将0X21 0X43 0X65 转换成 123456

ATHENA_EX void PrintHex(const void* buf, int size);     //打印16进制

ATHENA_EX RString operator^(const RString& sDir,const RString& sFile);      //拼接两个路径，即中间加SEP

/*
 *  正则表达式，是否匹配
 *  return
 *      true    匹配
 *      false   不匹配
 */
ATHENA_EX bool cmp_reg_exp( const char* pattern, const char* __str );

/***********************************
RList 类
***********************************/
template< class T >
class ATHENA_EX RList
{
public:
	typedef typename std::list< T, std::allocator< T > >::iterator Iterator;
	RList(){}
	~RList(){}

	int Count();
	T& operator[](int nIndex);
	Iterator Begin(){return m_List.begin();}
	Iterator End(){return m_List.end();}
	int Add(const T &stItem);
	void Delete(int nIndex);
	void Delete(const T& stItem);
	void Delete(Iterator Iter);
	void Clear();
	void Sort();
	bool Binary_Search(const T &stItem);
	bool Find(const T &stItem, int &nIndex);
	T* Find(const T &stItem);
	void Insert(int nIndex, const T &stItem);
	int FindInsertPos(const T &stItem, bool bUnique = true);
private:
	std::list< T > m_List;
};

/***********************************
RList类
***********************************/	
template< class T >
int RList< T >::Count()
{
	return m_List.size();
}

template< class T >
T& RList< T >::operator[](int nIndex)
{
	Iterator Iter = m_List.begin();
	advance(Iter, nIndex);
	return *Iter;
}

template< class T >
int RList< T >::Add(const T &stItem)
{
	m_List.push_back(stItem);
	return m_List.size();
}

template< class T >
void RList< T >::Delete(int nIndex)
{
	Iterator Iter = m_List.begin();
	advance(Iter, nIndex);
	m_List.erase(Iter);
}

template< class T >
void RList< T >::Delete(const T &stItem)
{
	Iterator Iter = find(m_List.begin(), m_List.end(), stItem);
	if(Iter != m_List.end())
		m_List.erase(Iter);
}

template< class T >
void RList< T >::Delete(Iterator Iter)
{
	m_List.erase(Iter);
}

template< class T >
void RList< T >::Clear()
{
	m_List.clear();
}

template< class T >
void RList< T >::Sort()
{
	m_List.sort();
}

template< class T >
bool RList< T >::Binary_Search(const T &stItem)
{
	return binary_search(m_List.begin(), m_List.end(), stItem);
}

template< class T >
bool RList< T >::Find(const T &stItem, int &nIndex)
{
	Iterator Iter = find(m_List.begin(), m_List.end(), stItem);
	if(Iter != m_List.end())
	{
		nIndex = 0;
		Iterator IterTmp = m_List.begin();
		while(IterTmp != Iter)
		{
			nIndex++;
			IterTmp++;	
		}
		
		return true;
	}
	else
		return false;
}

template< class T >
T* RList< T >::Find(const T &stItem)
{
	Iterator Iter = find(m_List.begin(), m_List.end(), stItem);
	if(Iter != m_List.end())
		return &(*Iter);
	else
		return NULL;
}

template< class T >
int RList< T >::FindInsertPos(const T &stItem, bool bUnique)
{
	int nHi, nLo, nMid, nState;
	T *pItem = NULL;
	
	nHi = Count()-1;
	nLo = 0;
	nMid = 0;
	nState = -2;
		
	while(nLo <= nHi)
	{
		nMid = (nLo+nHi)/2;
		pItem = &(*this)[nMid];
		if(*pItem > stItem)
		{
			nHi = nMid - 1;
			nState = -1;
		}
		else if(*pItem < stItem)
		{
			nLo = nMid + 1;
			nState = 1;
		}
		else
		{
			nState = 0;
			break;
		}
	}
	
	if(nState == 1)
		nMid += 1;
	else if(nState == 0)
	{
		if(bUnique)
			nMid = -1;
		else
			nMid += 1;
	}

	return nMid;
}

template< class T >
void RList< T >::Insert(int nIndex, const T &stItem)
{
	Iterator Iter = m_List.begin();
	advance(Iter, nIndex);
	m_List.insert(Iter, stItem);
}

/***********************************
RMap 类
***********************************/
template< class K, class T >
class ATHENA_EX RMap
{
public:
	typedef typename std::map< K, T, std::less< K > >::iterator Iterator;
	typedef typename std::map< K, T, std::less< K > >::value_type Pair;
	RMap(){};
	~RMap(){};

	int Count();
	T& operator[](const K &key);	
	bool Add(const K &key, const T &type);
	void Delete(const K &key);
	void Clear();
	bool Find(const K &key);
	bool Insert(int nIndex, const K key, const T &type);
	const K& GetKeyByIndex(int nIndex);
	T& GetValueByIndex(int nIndex);
private:
	std::map< K, T, std::less< K > > m_Map;
};

/***********************************
CMAP类
***********************************/
template< class K, class T >
int RMap< K, T >::Count()
{
	return m_Map.size();
}

template< class K, class T >
T& RMap< K, T >::operator[](const K &key)
{
	return m_Map[key];
}

template< class K, class T >
bool RMap< K, T >::Add(const K &key, const T &type)
{
	m_Map.insert( Pair(key, type) );
	return true;
}

template< class K, class T >
void RMap< K, T >::Delete(const K &key)
{
	Iterator Iter = m_Map.find(key);
	if(Iter != m_Map.end())
	{
		m_Map.erase(Iter);
	}
}

template< class K, class T >
void RMap< K, T >::Clear()
{
	m_Map.clear();
}

template< class K, class T >
bool RMap< K, T >::Find(const K &key)
{
	Iterator Iter = m_Map.find(key);
	return Iter != m_Map.end();
}

template< class K, class T >
bool RMap< K, T >::Insert(int nIndex, const K key, const T &type)
{
	Iterator Iter = m_Map.begin();
	advance(Iter, nIndex);
	m_Map.insert(Iter, Pair(key, type));
	return true;
}

template< class K, class T >
const K& RMap< K, T >::GetKeyByIndex(int nIndex)
{
	Iterator Iter = m_Map.begin();
	advance(Iter, nIndex);
	return Iter->first;
}

template< class K, class T >
T& RMap< K, T >::GetValueByIndex(int nIndex)
{
	Iterator Iter = m_Map.begin();
	advance(Iter, nIndex);
	return Iter->second;
}


}   //namespace

#endif
