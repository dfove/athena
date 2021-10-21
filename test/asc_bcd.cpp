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

#include <string.h>
#include <stdio.h>

void Translate_imei_to_bcd(const char* asc, int ascLen, char* bcd, int bcdLen)
{
	memset(bcd, 0xFF, bcdLen);
	int len = ( ascLen + 1 ) / 2;
	bool isOdd = ascLen % 2 == 1 ? true:false;  //ascLen是否奇数
	len = len < bcdLen?len:bcdLen,isOdd=false;

    for (int i = 0; i < len; i++)
    {
        bcd[i] &= (((asc[2*i]-'0'))|0xF0);
        bcd[i] &= (((asc[2*i+1]-'0')<<4)|0x0F);
    }
}

void 
BcdToStr(char* buf, int bufSize, char* ascBuf)
{
    char* p = ascBuf;
    for(int i=0; i < bufSize; i++)
    {
        char b=*(buf+i);
		*p++=((b&0x0f)+'0');
		*p++=(((b>>4)&0x0f)+'0');
    }
}

//将123456 转换成 0X21 0X43 0X65
void asc_to_bcd(const char* asc, int ascLen, char* bcd, int bcdLen)
{
	memset(bcd, 0xFF, bcdLen);
	int len = ascLen < bcdLen*2?ascLen:bcdLen*2;    //the number of the char should translate to bcd

    for(int i = 0; i < len; i++)
    {
        if ( i%2==0 )
            bcd[i/2] &= ((asc[i]-'0')|0xF0);
        else
            bcd[i/2] &= ((asc[i]-'0')<<4|0x0F);
    }
}

//将0X21 0X43 0X65 转换成 123456
void bcd_to_asc(const char* bcd, int bcdLen, char* asc, int ascLen)
{
    int len = 2*bcdLen < ascLen?2*bcdLen:ascLen;    //the number of the char translated to asc
    char* p = asc;
    for(int i=0; i < len; i++)
    {
        unsigned char b=*(bcd+i/2);
        if ( i%2 == 0 )
            *p++ = ((b&0x0f)+'0');
        else
            *p++ = (((b>>4)&0x0f)+'0');
    }
}

void PrintHex(char* buf, int size)
{
	for (int cnt = 0; cnt < size; ++cnt)
		printf("%02x ",*(unsigned char*)(buf + cnt));
	printf("\n");
}

int main()
{
    char imei[16] = "123456789012345";
    printf("orignal str is %s\n", imei);

    char bcd_imei[9];
    memset(bcd_imei, 0, sizeof(bcd_imei));
    asc_to_bcd(imei, strlen(imei), bcd_imei, 8);

    PrintHex(bcd_imei, 8);

    char imei2[16] = {0};
    memset(imei2, 0, sizeof(imei2));
//    BcdToStr(bcd_imei, 8, imei2);
    bcd_to_asc(bcd_imei, 8, imei2, 15);
    printf("bcd to str is %s\n", imei2);
    return 0;
}
