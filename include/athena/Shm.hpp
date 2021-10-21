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

#ifndef SHM_HPP_HEADER_INCLUDED
#define SHM_HPP_HEADER_INCLUDED

/***********************************
π≤œÌƒ⁄¥Ê¿‡
***********************************/
class CShm
{
public:
	CShm(int nKey, int nSize);

	virtual ~CShm();

    /*
     *  return
     *      -1 create or get semaphore error
     *      0  get existd semaphore success
     *      1  created semaphore success
     */
	int GetShm();

	bool AttachShm();

	bool DetachShm();

	bool RemoveShm();

	void InitShm();

	char* GetData();

protected:
	int m_nKey;

	int m_nSize;

	int m_nShmID;

	char* m_pData;

};  // class Shm

#endif
