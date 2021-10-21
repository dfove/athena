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

#include <Msg.hpp>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define	SVMSG_MODE	0640

CMsg::CMsg(int key, size_t maxMsgSize /*= MAX_MSG_SIZE*/)
          :m_nKey(key),
           m_nMsqID(-1),
           m_maxMsgSize(maxMsgSize)
{
    char* buf = new char[sizeof(MqBuf) + m_maxMsgSize];
    memset(buf, 0, sizeof(MqBuf) + m_maxMsgSize);
    m_msgBuf = (MqBuf*)buf;
    assert(m_msgBuf != NULL);
}

CMsg::~CMsg()
{
    
}

int 
CMsg::GetMsg()
{
	if((m_nMsqID = msgget(m_nKey, IPC_CREAT|IPC_EXCL|SVMSG_MODE)) < 0)
	{
		if(errno != EEXIST)
			return -1;

		if((m_nMsqID = msgget(m_nKey, IPC_CREAT|SVMSG_MODE)) < 0)
            return -1;
        else
            return 0;
	}
	return 1;
}

int 
CMsg::Send(const char* buf, size_t bufLen, unsigned priority /*= 1*/)
{
    if ( m_msgBuf == NULL || buf == NULL || priority == 0 )
        return -1;

    size_t sendLen = bufLen > m_maxMsgSize?m_maxMsgSize:bufLen;
    m_msgBuf->mtype = priority;
    memmove(m_msgBuf->mtext, buf, sendLen);
    int ret = msgsnd(m_nMsqID, m_msgBuf, sendLen, 0);
    return ret;
}

int 
CMsg::TrySend(const char* buf, size_t bufLen, unsigned priority /*= 1*/)
{
    if ( m_msgBuf == NULL || buf == NULL || priority == 0 )
        return -1;

    size_t sendLen = bufLen > m_maxMsgSize?m_maxMsgSize:bufLen;
    m_msgBuf->mtype = priority;
    memmove(m_msgBuf->mtext, buf, sendLen);
    int ret = msgsnd(m_nMsqID, m_msgBuf, sendLen, IPC_NOWAIT);
    if ( ret == -1 && errno == EAGAIN )
        return -2;
    return ret;
}

int 
CMsg::Recv(MqBuf** buf, unsigned priority /*= 0*/)
{
    if ( m_msgBuf == NULL || buf == NULL )
        return -1;

    int len = -1;
    memset(m_msgBuf->mtext, 0, m_maxMsgSize);
    len = msgrcv(m_nMsqID, m_msgBuf, m_maxMsgSize, priority, 0);
    if ( len == -1 )
        *buf = NULL;
    else
        *buf = m_msgBuf;
    return len;
}

int 
CMsg::TryRecv(MqBuf** buf, unsigned priority /*= 0*/)
{
    if ( m_msgBuf == NULL )
        return -1;

    int len = -1;
    memset(m_msgBuf->mtext, 0, m_maxMsgSize);
    len = msgrcv(m_nMsqID, m_msgBuf, m_maxMsgSize, priority, IPC_NOWAIT);
    if ( len == -1 )
    {
        *buf = NULL;
        if ( errno == EAGAIN || errno == ENOMSG )
            return -2;
    }
    else
        *buf = m_msgBuf;
    return len;
}

int 
CMsg::Size() const
{
    if ( m_nMsqID != -1 )
    {
        struct msqid_ds buf;
        int ret = msgctl(m_nMsqID, IPC_STAT, &buf);
        if ( ret == 0 )
            return buf.msg_qnum;
    }
    return 0;
}

bool 
CMsg::Remove()
{
    if ( m_nMsqID != -1 )
    {
        int iRet = msgctl(m_nMsqID, IPC_RMID, NULL);
        return iRet==-1?false:true;
    }
    return true;
}
