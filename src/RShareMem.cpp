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

#include <RShareMem.hpp>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

namespace athena
{
#if defined(WIN32) || defined(_WIN32_WCE)
    #include <windows.h>
    typedef struct
    {
        HANDLE h;
    }
    Shm_t;
#elif defined(HAVE_SYS_MMAN_H)  /*Posix*/
	#include <unistd.h>
    #include	<fcntl.h>		/* for nonblocking and O_RDWR | O_CREAT*/
    #define	FILE_MODE	(S_IRUSR | S_IWUSR | S_IRGRP)
					    /* default permissions for new files 0x640*/

    #include <sys/mman.h>
    typedef int Shm_t;
#elif defined(HAVE_SYS_SHM_H)   /*System V*/
	#include <unistd.h>
    #include <sys/shm.h>
    #define	SVSHM_MODE	0640
    typedef int Shm_t;
#endif
}

using namespace athena;

RShareMem::RShareMem(const char* name /*= NULL*/, 
                     unsigned int size /*= 0*/, 
                     bool blfilemap /*= true*/)
          :m_shm(NULL),
           m_blfilemap(blfilemap),
           m_mem(NULL)
{
    Shm_t* shm = new Shm_t;
    if ( name != NULL )
        m_name = name;
#if defined (WIN32) || defined (_WINDOWS)
    HANDLE hFile = NULL;
    DWORD memsize = size;
    if ( !m_name.empty()  )
    {
        hFile = CreateFile(m_name, 
                           GENERIC_READ | GENERIC_WRITE,        //Access to the object.
                           FILE_SHARE_READ | FILE_SHARE_WRITE,  //Sharing mode of the object.
                           NULL,                                //whether can be inherited
                           OPEN_ALWAYS,                         //Action to take on files that exist or not exist.
                           FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED, 
                           NULL);
        if ( hFile != NULL && GetLastError() == ERROR_ALREADY_EXISTS )  //�ļ��Ѵ���
        {
            int fsize = GetFileSize(hFile, NULL);
            if ( fsize == 0 && size == 0)       //�ļ���СΪ0�ᵼ��CreatFileMapping()ʧ��
            {
                memsize = 1;            //����Ѵ��ڵ��ļ���СΪ0���򽫴򿪴�С����Ϊ1�ֽ�
            }
        }
    }
    else
    {
        if ( memsize == 0 )             //���������ڴ��С����Ϊ1
            memsize = 1;
    }
    shm->h = CreateFileMapping(hFile,    // current file handle 
                                NULL,                              // default security 
                                PAGE_READWRITE,                    // read/write permission 
                                memsize,                           // max. object size 
                                memsize,                           // size of hFile 
                                NULL);                             // name of mapping object
    if ( hFile != NULL )
        CloseHandle(hFile);
    if ( shm->h == NULL )         //�������filemapʧ��
    {
        m_mem = NULL;
        delete shm;
        shm = NULL;
    }
    else
    {
        m_mem =  MapViewOfFile(shm->h,                           // handle to mapping object 
                                FILE_MAP_ALL_ACCESS,               // read/write permission 
                                0,                                 // max. object size 
                                0,                                 // size of hFile 
                                0);                                // map entire file 
        if ( m_mem == NULL )        //�������mapviewʧ��
        {
            CloseHandle(shm->h);
            m_mem = NULL;
            delete shm;
            shm = NULL;
        }
    }
#elif defined (HAVE_SYS_MMAN_H)         //POSIX SHM
    if ( !m_name.empty() )                 //���������ڴ棨ӳ���ļ�����posix shm��
    {
        int fd = -1, memsize = size;
        bool binit = true;         //�Ƿ���Ҫ��ʼ��Ϊ0�������Ѵ��ڵĲ��ó�ʼ�����´�������Ҫ��ʼ��
        int oflags = O_RDWR | O_CREAT;
        if ( blfilemap == true )            //���ļ�ӳ��
        {
            fd = open(m_name, oflags | O_EXCL , FILE_MODE);
            if ( fd == -1 && errno == EEXIST && (fd = open(m_name, O_RDWR)) != -1 )  //�ļ������Ҵ��ļ��ɹ�
            {
                binit = false;         //����Ҫ��ʼ��
                struct stat buf;
                if ( fstat(fd, &buf) == 0 ) //��ȡ�ļ���С
                {
                    if ( size == 0 )        //�ļ������Ҳ���sizeΪ0�������ڴ��С�����ļ���С�����������Ϊsize��С
                    {
                        memsize = buf.st_size;;
                    }
                }
                else
                {
                    close(fd);
                    fd = -1;
                }
            }
        }
        else                                //���ڴ�ӳ��
        {
            if ( *name != '/' )
            {
                m_name.Format("%s%s", POSIX_IPC_PREFIX, name);
            }
            fd = shm_open(m_name, oflags | O_EXCL, FILE_MODE);
            if ( fd == -1 && errno == EEXIST && (fd = shm_open(m_name, oflags, FILE_MODE)) != -1 )
            {
                binit = false;      //���Ѵ��ڵĹ����ڴ治��Ҫ��ʼ��
            }
        }
        if ( fd != -1 )
        {
            if ( memsize == 0 )                //�����ڴ���СΪ1���ֽڴ�С
                memsize = 1;
            ftruncate(fd, memsize);
            m_mem = mmap(NULL, memsize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
            if ( MAP_FAILED != m_mem && binit == true )
            {
                memset(m_mem, 0, memsize);
            }
            *shm = memsize;     //����munmap(void* addr, size_t len)����Ҫ�õ���len����
            close(fd);
        }
    }
    else        //���������ڴ棬��fork�ӽ��̵�ʱ��ʹ�ã��ڴ��Զ���ʼ��Ϊ0
    {
        int memsize = size;
        if ( memsize == 0 )
            memsize = 1;
        *shm = memsize;
        #ifdef MAP_ANONYMOUS
            m_mem = mmap(NULL, memsize, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
//        #else
//            int fd = -1;
//            if ( (fd = open(m_name, O_RDWR)) != -1  )
//            {
//                m_mem = mmap(NULL, memsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//            }
        #endif
    }
    if ( NULL == m_mem || MAP_FAILED == m_mem )
    {
        m_mem = NULL;
        delete shm;
        shm = NULL;
    }
#elif defined (HAVE_SYS_SHM_H)          // SYSTEM V SHM
    int len = size;
    if ( len == 0 )
        len = 1;
    int oflag = IPC_CREAT | IPC_EXCL | SVSHM_MODE;
    if ( !m_name.empty() )
    {
        *shm = shmget(ftok(m_name, 0), len, oflag);   //name : pathname
        if ( *shm == -1 && errno == EEXIST )
        {
            *shm = shmget(ftok(m_name, 0), len, 0);
        }
    }
    else
    {
        *shm = shmget(IPC_PRIVATE, len, oflag);
    }
    if ( *shm == -1 )
    {
        delete shm;
        shm = NULL;
    }
    else
    {
        m_mem = shmat(*shm, NULL, 0);
        if ( (void *)-1 == m_mem )
        {
           m_mem = NULL;
           delete shm;
           shm = NULL;
        }
    }
#endif 
    m_shm = (struct RShm_t*)shm;
}

RShareMem::~RShareMem()
{
    if ( m_shm != NULL )
    {
        Shm_t* shm = (Shm_t*)m_shm;
#if defined (WIN32) || defined (_WINDOWS)
        CloseHandle(shm->h);
        if ( m_mem != NULL )
            UnmapViewOfFile(m_mem);
#elif defined (HAVE_SYS_MMAN_H)
        if ( m_mem != NULL )
            munmap(m_mem, *shm);
#elif defined (HAVE_SYS_SHM_H)
        if ( m_mem != NULL )
        {
            shmdt(m_mem);
            m_mem = NULL;
        }
#endif
        delete shm;
        shm = NULL;
        m_shm = NULL;
    }
}

void* 
RShareMem::GetShm() const
{
    return m_mem;
}

const RString& 
RShareMem::GetName() const
{
    return m_name;
}

bool 
RShareMem::Remove()
{
    if ( m_shm == NULL || m_name.empty() )
        return true;

#if defined (WIN32) || defined (_WINDOWS)
    /*
     *   ɾ����ӳ����ļ�
     *   �����ڴ�ֻҪUnmapViewOfFile() 
     */ 
    if ( DeleteFile(m_name.c_str()) == FALSE)
        return false;
#elif defined (HAVE_SYS_MMAN_H)
    /*
     *  �ļ�ӳ���POSIX SHMӳ���ʱ�����Ҫɾ��ϵͳ�ļ���POSIX SHM[shm_open()��]
     *  �����ڴ�ֻҪunmap()
     */
    if ( m_blfilemap == true )
    {
        unlink(m_name.c_str());
    }
    else
        shm_unlink(m_name.c_str());
#elif defined (HAVE_SYS_SHM_H)
    /*
     *  System V ipc����ʹ���ļ�ϵͳ�е�·������ʶ�ģ����ʹ�ñ�׼��ls��rm�޷��������ǣ�Ҳ�޷�ɾ������
     *  ���������ipcs��ipcrm
     */
    Shm_t* shm = (Shm_t*)m_shm;
    shmdt(m_mem);
    m_mem = NULL;
    if ( shmctl(*shm, IPC_RMID, NULL) == -1 )
        return false;
#endif
    return true;
}
