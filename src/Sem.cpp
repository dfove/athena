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

#include <Sem.hpp>
#include <errno.h>
#include <sys/sem.h>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
  int val;                    /* value for SETVAL */
  struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
  unsigned short int *array;  /* array for GETALL, SETALL */
  struct seminfo *__buf;      /* buffer for IPC_INFO */
};
#endif


CSem::CSem(int nKey, int nCount):
	m_nKey(nKey),
	m_nSemID(-1),
	m_nSemCount(nCount)
{
}

CSem::~CSem()
{
    
}

int 
CSem::GetSem()
{
	if((m_nSemID = semget(m_nKey, m_nSemCount, IPC_CREAT|IPC_EXCL|0660)) < 0)
	{
		if(errno != EEXIST)
			return -1;

		if((m_nSemID = semget(m_nKey, m_nSemCount, IPC_CREAT|0660)) < 0)
			return -1;
	    else
	    {
	        return 0;
	    }
	}
    if ( InitSem() == false )
        return -1;
    return 1;
}

bool CSem::RemoveSem()
{
	union semun unSem;
	if (semctl(m_nSemID, 0, IPC_RMID, unSem) < 0)
		return false;
    return true;
}

bool CSem::Lock(int nSemNum /*= 0*/)
{
	struct sembuf SemBuf[1];
	
	SemBuf[0].sem_num = nSemNum;
	SemBuf[0].sem_op = -1;
	SemBuf[0].sem_flg = SEM_UNDO;
	
	if(semop(m_nSemID, SemBuf, 1) < 0)
	{
		if (errno == EAGAIN || errno == EINTR) 
			return false;
	}
	return true;
}

bool CSem::Unlock(int nSemNum /*= 0*/)
{
	struct sembuf SemBuf[1];

	SemBuf[0].sem_num = nSemNum;
	SemBuf[0].sem_op = 1;
	SemBuf[0].sem_flg = SEM_UNDO;

	if(semop(m_nSemID, SemBuf, 1) < 0)
	{
		if (errno == EAGAIN || errno == EINTR) 
			return false;
	}
	return true;
}

bool CSem::InitSem()
{
	unsigned short int *pValue = new unsigned short int [m_nSemCount];
	union semun unSem;

	for (int i = 0; i < m_nSemCount; i++)
		pValue[i] = (unsigned short int)1;

	unSem.array = pValue;
	if(semctl(m_nSemID, 0, SETALL, unSem) < 0)
	{
		delete[] pValue;
		return false;
	}

	delete[] pValue;
	return true;
}

bool CSem::SetValue(int nSemNum, int nValue)
{
	union semun unSem;

	unSem.val = nValue;
	if(semctl(m_nSemID, nSemNum, SETVAL, unSem) < 0)
	{
	    return false;
	}
	return true;
}

int CSem::GetValue(int nSemNum /*= 0*/)
{
	union semun unSem;

	return semctl(m_nSemID, nSemNum, GETVAL, unSem);
}
