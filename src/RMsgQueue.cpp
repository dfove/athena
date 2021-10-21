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

#include <RMsgQueue.hpp>
#include <RStream.hpp>
#include <stdlib.h>
#include <stdio.h>

namespace athena
{
#if defined (WIN32) || defined (_WINDOWS)
    #include <windows.h>
    typedef struct
    {
        HANDLE h;
    }
    Msg_t;
#elif defined(HAVE_MQUEUE_H)
    #include	<fcntl.h>		/* for nonblocking and O_RDWR | O_CREAT*/
    #define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP)
					    /* default permissions for new files 0x640*/
    #include <sys/time.h>
    #include <mqueue.h>
    typedef mqd_t Msg_t;
#elif defined(HAVE_SYS_MSG_H)
    #include <sys/msg.h>
  #ifndef _GNU_SOURCE
    struct msgbuf {
                 long mtype;     /* message type, must be > 0 */
                 char mtext[1];  /* message data */
            };
  #endif
    typedef int Msg_t;
    #define	SVMSG_MODE	0640
    #define	MAXMSG	8192
#endif
}

using namespace athena;

RMsgQueue::RMsgQueue()
          :m_msg(NULL)
{

}

RMsgQueue::RMsgQueue(unsigned key)
          :m_msg(NULL)
{
    Init(key);
}

RMsgQueue::RMsgQueue(const char* name)
          :m_msg(NULL)
{
    Init(name);
}

RMsgQueue::~RMsgQueue()
{
    if ( m_msg != NULL )
    {
        Msg_t* msg = (Msg_t*)m_msg;
#if defined (WIN32) || defined (_WINDOWS)
#elif defined(HAVE_MQUEUE_H)
        mq_close(*msg);
#elif defined(HAVE_SYS_MSG_H)

#endif
        delete msg;
        msg = NULL;
        m_msg = NULL;
    }
}

void 
RMsgQueue::Init(unsigned key)
{
    if ( m_msg != NULL )    //已初始化
        return;

    Msg_t* msg = new Msg_t;
#if defined (WIN32) || defined (_WINDOWS)

#elif defined(HAVE_MQUEUE_H)
    m_name.Format("%s%d", POSIX_IPC_PREFIX, key);
    *msg = mq_open(m_name.c_str(), O_RDWR | O_CREAT, FILE_MODE, NULL);
    if ( (mqd_t)-1 == *msg )
    {
        perror("the error is");
        delete msg;
        msg = NULL;
    }
#elif defined(HAVE_SYS_MSG_H)
    int oflags = SVMSG_MODE | IPC_CREAT;
    *msg = msgget(key, oflags);
    if ( *msg == -1 )
    {
        delete msg;
        msg = NULL;
    }
#endif
    m_msg = (struct RMsg_t*)msg;
}

void 
RMsgQueue::Init(const RString& name)
{
    if ( m_msg != NULL )    //已初始化
        return;

    if ( name != NULL )
        m_name = name;
    Msg_t* msg = new Msg_t;
#if defined (WIN32) || defined (_WINDOWS)

#elif defined(HAVE_MQUEUE_H)
    if ( name == NULL )
    {
        delete msg;
        msg = NULL;
    }
    else
    {
        if ( *name != '/' )
        {
            m_name.Format("%s%s", POSIX_IPC_PREFIX, name.c_str());
        }
        *msg = mq_open(m_name.c_str(), O_RDWR | O_CREAT, FILE_MODE, NULL);
        if ( (mqd_t)-1 == *msg )
        {
            perror("the error is");
            delete msg;
            msg = NULL;
        }
    }
#elif defined(HAVE_SYS_MSG_H)
    int oflags = SVMSG_MODE | IPC_CREAT;
    if ( m_name.empty() )
        *msg = msgget(ftok(IPC_PRIVATE, 0), oflags);
    else
        *msg = msgget(ftok(name, 0), oflags);
    if ( *msg == -1 )
    {
        delete msg;
        msg = NULL;
    }
#endif
    m_msg = (struct RMsg_t*)msg;
}

bool 
RMsgQueue::Send(const char* buf, size_t msgsize, unsigned priority /*= 1*/)
{
    if ( m_msg == NULL || buf == NULL)
        return false;

    Msg_t* msg = (Msg_t*)m_msg;
#if defined (WIN32) || defined (_WINDOWS)
    return false;
#elif defined(HAVE_MQUEUE_H)
    int ret = mq_send(*msg, buf, msgsize, priority);
    if ( ret == -1 )
        return false;
#elif defined(HAVE_SYS_MSG_H)
    if ( priority == 0 )
        return false;
    char* ptr = (char*)malloc(sizeof(long) + msgsize);
    memset(ptr, 0, sizeof(long) + msgsize);
    struct msgbuf* tmp = (struct msgbuf*)ptr;
#if defined(_SUNOS_) && defined(_XOPEN_SOURCE)
    tmp->_mtype = priority;
#else
    tmp->mtype = priority;
#endif
    memmove(ptr+sizeof(long), buf, msgsize);
    int ret = msgsnd(*msg, tmp, msgsize, 0);
    free(ptr);
    if ( ret == -1 )
        return false;
#endif
    return true;
}

bool 
RMsgQueue::Send(const RStream& stream, unsigned priority /*= 1*/)
{
    if ( m_msg == NULL )
        return false;
    return Send(stream.Data(), stream.Size(), priority);
}

bool 
RMsgQueue::TrySend(const char* buf, size_t msgsize, unsigned priority /*= 1*/)
{
    if ( m_msg == NULL || buf == NULL || priority == 0)
        return false;

    Msg_t* msg = (Msg_t*)m_msg;
#if defined (WIN32) || defined (_WINDOWS)
    return false;
#elif defined(HAVE_MQUEUE_H)
    struct timespec t;
    t.tv_sec=0;
    t.tv_nsec=100; //100纳秒
    return mq_timedsend(*msg, buf, msgsize, priority, &t);
#elif defined(HAVE_SYS_MSG_H)
    char* ptr = (char*)malloc(sizeof(long) + msgsize);
    memset(ptr, 0, sizeof(long) + msgsize);
    struct msgbuf* tmp = (struct msgbuf*)ptr;
#if defined(_SUNOS_) && defined(_XOPEN_SOURCE)
    tmp->_mtype = priority;
#else
    tmp->mtype = priority;
#endif
    memmove(ptr+sizeof(long), buf, msgsize);
    int ret = msgsnd(*msg, tmp, msgsize, IPC_NOWAIT);
    free(ptr);
    if ( ret == -1 )
        return false;
#endif
    return true;
}

bool 
RMsgQueue::TrySend(const RStream& stream, unsigned priority /*= 1*/)
{
    if ( m_msg == NULL )
        return false;
    return TrySend(stream.Data(), stream.Size(), priority);
}

int 
RMsgQueue::Recv(char* buf, size_t maxmsgsize, unsigned priority /*= 0*/)
{
    if ( m_msg == NULL || buf == NULL || maxmsgsize == 0 )
        return false;

    int len = -1;

    Msg_t* msg = (Msg_t*)m_msg;
#if defined (WIN32) || defined (_WINDOWS)

#elif defined(HAVE_MQUEUE_H)
    struct mq_attr attr;
    if ( mq_getattr(*msg, &attr) == -1 )        //获取消息的最大长度,字节
    {
  #if defined (_DEBUG)
        perror("RMsgQueue::Recv() eror");
  #endif
        return -1;                      //获取最大消息长度出错
    }  
    len = mq_receive(*msg, buf, attr.mq_msgsize, NULL);

    if ( len == -1 )
    {
  #if defined (_DEBUG)
        perror("the error is ");
  #endif
        return -1;
    }
    return len;
#elif defined(HAVE_SYS_MSG_H)
    char* ptr = (char*)malloc(sizeof(long) + maxmsgsize + 1);
    memset(ptr, 0, sizeof(long) + maxmsgsize);
    struct msgbuf* tmp = (struct msgbuf*)ptr;
    len = msgrcv(*msg, tmp, maxmsgsize, priority, 0);
    if ( len == -1 )
    {
        free(ptr);
        return -1;
    }
#if defined(_DEBUG)
  printf("%s,%d: recv %d bytes, msgtype is: %ld, msg is: ", __FILE__, __LINE__, len, tmp->mtype);
  char* optr = ptr+sizeof(long);
  for ( int i = 0; i < len; i++ )
    putchar(*optr++);
  printf("\n");
#endif
    strncpy(buf, ptr+sizeof(long), len);
    free(ptr);
#endif
    return len;
}

int 
RMsgQueue::Recv(RStream& stream, unsigned priority /*= 0*/)
{
    if ( m_msg == NULL )
        return -1;

#if defined (WIN32) || defined (_WINDOWS)
    return -1;
#elif defined(HAVE_MQUEUE_H)
    Msg_t* msg = (Msg_t*)m_msg;
    struct mq_attr attr;
    if ( mq_getattr(*msg, &attr) == -1 )        //获取消息的最大长度,字节
    {
  #if defined (_DEBUG)
        perror("RMsgQueue::Recv() eror");
  #endif
        return -1;                      //获取最大消息长度出错
    }
    int size = attr.mq_msgsize;
    char* buf = (char*)malloc(size);
    int len = mq_receive(*msg, buf, size, NULL);
    if ( len == -1 )
    {
  #if defined (_DEBUG)
        perror("the error is ");
  #endif
        free(buf);
        return -1;
    }
    stream = RStream(buf, len);
    free(buf);
    return len;
#elif defined(HAVE_SYS_MSG_H)
    char buf[MAXMSG];
    int len = Recv(buf, MAXMSG, priority);
    if ( len == -1 )
    {
        return -1;
    }
  #if defined(_DEBUG)
    printf("%s,%d: recv %d bytes, msg is: %s\n", __FILE__, __LINE__, len, buf);
  #endif
    stream.Assign(buf, len);
    return len;
#endif
    return -1;
}

int 
RMsgQueue::TryRecv(char* buf, size_t maxmsgsize, unsigned priority /*= 0*/)
{
    if ( m_msg == NULL || buf == NULL || maxmsgsize == 0 )
        return false;

    int len = -1;

    Msg_t* msg = (Msg_t*)m_msg;
#if defined (WIN32) || defined (_WINDOWS)
    return -1;
#elif defined(HAVE_MQUEUE_H)
    struct mq_attr attr;
    if ( mq_getattr(*msg, &attr) == -1 )        //获取消息的最大长度,字节
    {
      #if defined (_DEBUG)
            perror("RMsgQueue::Recv() eror");
      #endif
        return -1;                      //获取最大消息长度出错
    }

    struct timespec ts;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + tv.tv_usec/10000000;
    ts.tv_nsec = ( tv.tv_usec % 1000000 ) * 1000;
    len = mq_timedreceive(*msg, buf, attr.mq_msgsize, NULL, &ts);

    if ( len == -1 )
    {
      #if defined (_DEBUG)
            perror("the error is ");
      #endif
        return -1;
    }
    return len;
#elif defined(HAVE_SYS_MSG_H)
    char* ptr = (char*)malloc(sizeof(long) + maxmsgsize + 1);
    memset(ptr, 0, sizeof(long) + maxmsgsize);
    struct msgbuf* tmp = (struct msgbuf*)ptr;
    len = msgrcv(*msg, tmp, maxmsgsize, priority, IPC_NOWAIT);
    if ( len == -1 )
    {
        free(ptr);
        return -1;
    }
  #if defined(_DEBUG)
    printf("%s,%d: recv %d bytes, msgtype is: %ld, msg is: ", __FILE__, __LINE__, len, tmp->mtype);
    char* optr = ptr+sizeof(long);
    for ( int i = 0; i < len; i++ )
    putchar(*optr++);
    printf("\n");
  #endif
    strncpy(buf, ptr+sizeof(long), len);
    free(ptr);
#endif
    return len;
}

int 
RMsgQueue::TryRecv(RStream& stream, unsigned priority /*= 0*/)
{
    if ( m_msg == NULL )
        return -1;

#if defined (WIN32) || defined (_WINDOWS)
    return -1;
#elif defined(HAVE_MQUEUE_H)
    Msg_t* msg = (Msg_t*)m_msg;
    struct mq_attr attr;
    if ( mq_getattr(*msg, &attr) == -1 )        //获取消息的最大长度,字节
    {
      #if defined (_DEBUG)
        perror("RMsgQueue::Recv() eror");
      #endif
        return -1;                      //获取最大消息长度出错
    }
    int size = attr.mq_msgsize;
    char* buf = (char*)malloc(size);
    struct timespec ts;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec + tv.tv_usec/10000000;
    ts.tv_nsec = ( tv.tv_usec % 1000000 ) * 1000;
    int len = mq_timedreceive(*msg, buf, size, NULL, &ts);
    if ( len == -1 )
    {
      #if defined (_DEBUG)
        perror("the error is ");
      #endif
        free(buf);
        return -1;
    }
    stream = RStream(buf, len);
    free(buf);
    return len;
#elif defined(HAVE_SYS_MSG_H)
    char buf[MAXMSG];
    int len = TryRecv(buf, MAXMSG, priority);
    if ( len == -1 )
    {
        return -1;
    }
#if defined(_DEBUG)
  printf("%s,%d: recv %d bytes, msg is: %s\n", __FILE__, __LINE__, len, buf);
#endif
    stream.Assign(buf, len);
    return len;
#endif
    return -1;
}

const RString& 
RMsgQueue::Name() const
{
    return m_name;
}

int 
RMsgQueue::Size() const
{
    if ( m_msg != NULL )
    {
        Msg_t* msg = (Msg_t*)m_msg;
#if defined (WIN32) || defined (_WINDOWS)
#elif defined(HAVE_MQUEUE_H)
        struct mq_attr attr;
        int ret = mq_getattr(*msg, &attr);
        if ( ret == 0 )
            return attr.mq_curmsgs;
#elif defined(HAVE_SYS_MSG_H)
        struct msqid_ds buf;
        int ret = msgctl(*msg, IPC_STAT, &buf);
        if ( ret == 0 )
            return buf.msg_qnum;
#endif
    }
    return 0;
}

bool 
RMsgQueue::Remove()
{
    if ( m_msg != NULL )
    {
        
#if defined (WIN32) || defined (_WINDOWS)
#elif defined(HAVE_MQUEUE_H)
        if ( !m_name.empty() )
        {
            mq_unlink(m_name.c_str());
        }
#elif defined(HAVE_SYS_MSG_H)
        Msg_t* msg = (Msg_t*)m_msg;
        msgctl(*msg, IPC_RMID, NULL);
#endif
    }
    return true;
}
