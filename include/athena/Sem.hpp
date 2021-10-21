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

#ifndef SEM_HPP_HEADER_INCLUDED
#define SEM_HPP_HEADER_INCLUDED

/***********************************
–≈∫≈¡ø¿‡
***********************************/
class CSem
{
public:
	CSem(int nKey, int nCount = 1);

	~CSem();

	bool Lock(int nIndex = 0);

	bool Unlock(int nIndex = 0);

    /*
     *  return
     *      -1 create or get semaphore error
     *      0  get existd semaphore success
     *      1  created semaphore success
     */
	int GetSem();

	bool RemoveSem();

	bool SetValue(int nSemNum, int nValue);

	int GetValue(int nSemNum = 0);

private:
    bool InitSem();

private:
	int m_nKey;

	int m_nSemID;

	int m_nSemCount;

};


#endif /*SEM_HPP_HEADER_INCLUDED;*/
