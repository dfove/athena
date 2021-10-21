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

#include <Shm.hpp>
#include <sys/shm.h>
#include <errno.h>
#include <string.h>

CShm::CShm(int nKey, int nSize):
	m_nKey(nKey),
	m_nSize(nSize),
	m_nShmID(-1),
	m_pData(NULL)
{
}

CShm::~CShm()
{
    
}

int 
CShm::GetShm()
{
	if((m_nShmID = shmget(m_nKey, m_nSize, IPC_CREAT|IPC_EXCL|0660)) < 0)
	{
		if(errno != EEXIST)
			return -1;

		if((m_nShmID = shmget(m_nKey, m_nSize, IPC_CREAT|0660)) < 0)	
            return -1;
        else
            return 0;
	}
	return 1;
}

bool 
CShm::AttachShm()
{
	if((m_pData = (char*)shmat(m_nShmID, NULL, 0)) < 0)
		return false;
    return true;
}

bool 
CShm::DetachShm()
{
	if ( shmdt(m_pData) < 0 )
	    return false;
    return true;
}

bool 
CShm::RemoveShm()
{
	if(shmctl(m_nShmID, IPC_RMID, NULL) < 0)
		return false;
    return true;
}

void 
CShm::InitShm()
{
	memset(m_pData, 0, m_nSize);
}

char* 
CShm::GetData()
{
    return m_pData;
}
