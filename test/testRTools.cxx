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

//g++ -o countDPC_OPC_SLC testRTools.cxx 

#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string>
#include <regex.h>
#include <sys/stat.h>
#include <map>

using namespace std;

struct TPcapHeader {
	unsigned int MagicNum;
	unsigned short MajorVer;
	unsigned short MinorVer;
	unsigned int TimeZone;	//未使用
	unsigned int TimeStamp;	//未使用
	unsigned int SnapshotLen;
	unsigned int LinkLayerType;
};

struct TFrameHeader {
	unsigned int TimeOffset;	//秒
	unsigned int TimeAccuracy;	//微秒
	unsigned int CapturedLen;
	unsigned int PacketLen;
};

struct TMTP3Hdr { //message signal unit MTP3 header
//	TMTP2Hdr L2Hdr;
	union { 
		struct {
			unsigned char SI:4;//Service Indicator
			unsigned char SSF:4;//Sub-Service Field
		} SIO_b;
		unsigned char SIO_B;
	};
};

union TRouteIndInt { //Route Indicator internation
	struct{
	unsigned int DPC:14;  //destinate point code
	unsigned int OPC:14;  //originate point code
	unsigned int SLC:4;
	};
	struct{
	unsigned int DOPC:28;
	unsigned int SLC2:4;
	};
	unsigned int RouteId;
};

bool cmp_reg_exp( const char* pattern, const char* str )
{
    int    status;
    regex_t    re;

    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB) != 0) 
    {
      return false;      /* Report error. */
    }
    status = regexec(&re, str, (size_t) 0, NULL, 0);
    regfree(&re);
    if (status != 0)
    {
        return false;      /* Report error. */
    }
    return true;
}

static map<int, int> mCount;    //TRouteIndInt -- > count

static map< int, map<int, int> > dopcCount; //TRouteIndInt.DOPC -- > (TRouteIndInt.SLC -- > count)

void ProcessFile(const string& filename, int fileLength)
{
    printf("filename %s, len:%d\n", filename.c_str(), fileLength);
    FILE* fp = fopen(filename.c_str(),"r");
    if ( fp == NULL )
    {
        printf("The file %s is not exist!\n", filename.c_str());
        return;
    }
    char* pBuf = new char[fileLength];
    fread(pBuf, sizeof(char), fileLength, fp); //将剩余的文件内容全部读入内存
    char* pTmp = pBuf + sizeof(TPcapHeader);
    
    int i = 0;
    while (pTmp - pBuf < fileLength)
    {
        char* pNum = pTmp + sizeof(TFrameHeader) + sizeof(TMTP3Hdr);
        TRouteIndInt* num = (TRouteIndInt*)pNum;
        mCount[num->RouteId]++;
        dopcCount[num->DOPC][num->SLC2]++;
//        printf("DPC:%d, OPC:%d, SLS:%d\n", num->DPC, num->OPC, num->SLC);
        TFrameHeader* pEachHead = (TFrameHeader*)pTmp;
        pTmp += (sizeof(TFrameHeader) + pEachHead->PacketLen);
        i++;
    }
    printf("total %d, mCount size is %d\n", i, mCount.size());
    delete[] pBuf;
    fclose(fp);
}

void PrintMap(const map<int, int>& mc)
{
    map<int, int>::const_iterator iter;
    for (iter = mc.begin(); iter != mc.end(); iter++)
    {
        TRouteIndInt* f1 = (TRouteIndInt*)(&iter->first);
        int f2 = iter->second;
        printf("DPC %d ----> OPC %d SLC %d, total %d\n", f1->DPC, f1->OPC, f1->SLC, f2);
    }
}

void PrintMap2(const map< int, map<int, int> >& mc)
{
    map< int, map<int, int> >::const_iterator iter;
    for (iter = mc.begin(); iter != mc.end(); iter++)
    {
        TRouteIndInt* f1 = (TRouteIndInt*)(&iter->first);
        map<int, int> f2 = iter->second;
//        printf("OPC %d->DPC %d ", f1->OPC, f1->DPC);
        printf("%d->%d ", f1->OPC, f1->DPC);
        map<int, int>::const_iterator iter2;
        for (int i = 0; i < 16; i++)
        {
            f2[i] = f2[i] + 0;
        }
        for (iter2 = f2.begin(); iter2 != f2.end(); iter2++)
        {
            int f3 = iter2->first;
            int f4 = iter2->second;
            printf("%d:%-5d ", f3, f4);
        }
        printf("\n");
    }
}

int main(int argc, char** argv)
{
	struct dirent *pDirent = NULL;
	struct stat statbuf;
	DIR * pDir = NULL;

    if (argc != 2)
    {
        printf("Usage:%s path\n", argv[0]);
        return 0;
    }

    string strScanPath(argv[1]);
	if( (pDir = opendir(strScanPath.c_str())) == NULL)
	{
        printf("Open the path error:%s, errno:%d, %s!", 
              strScanPath.c_str(), errno, strerror(errno)); 
	    return false;
	}

    int n = 0;
    string Regex(".*\\.pcap");
	while((pDirent = readdir(pDir)) != NULL)
	{
		if(strcmp(pDirent->d_name,".") ==0 || strcmp(pDirent->d_name,"..") ==0)
			continue;
	    string filename = pDirent->d_name;
		string absFileName = strScanPath +"/"+ pDirent->d_name;
		if(stat(absFileName.c_str(), &statbuf) < 0)
		{
			break;
		}
		if(S_ISREG(statbuf.st_mode))
		{
            if ( cmp_reg_exp(Regex.c_str(), filename.c_str()) == true )
            {
               ProcessFile(absFileName, statbuf.st_size);
            }
		}
	}
	PrintMap2(dopcCount);
	closedir(pDir);

    return 0;
}
