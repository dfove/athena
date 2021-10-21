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

#ifndef __TRIPLE_DES__
#define	__TRIPLE_DES__

#define DATA_DEC	0		// 解密标志
#define DATA_ENC	1		// 加密标志

#define MODE_ECB	0		// ECB模式
#define MODE_CBC	1		// CBC模式

/*----------------------------------------------------------------------------------------------
** 参数：
  buffin              [in]      输入数据
  inlen               [in]      输入数据的长度
  key1, key2, key3    [in]      子密钥，每个子密钥8个字节
  buffout             [in]      输出缓冲区，由调用程序分配空间，ECB模式下输出缓冲区最多比
  				输入缓冲区大8个字节；CBC模式下输出缓冲区最多比输入缓冲区大16字节
  outlen              [in,out]  输出缓冲区的长度，长度不够时，函数返回失败并填充实际需要的长度
  enc                 [in]      1表示加密，0表示解密
  cbc                 [in]      1表示CBC模式，0表示ECB模式

** 返回：
  1    成功
  0    失败
----------------------------------------------------------------------------------------------*/
int TripleDES(unsigned char* buffin, unsigned long inlen, unsigned char key1[8], 
		unsigned char key2[8], unsigned char key3[8], unsigned char* buffout, 
		unsigned long* outlen, int enc, int cbc) ;

// 支持两个不同的子密钥
#define TriDes2Keys(buffin, inlen, key1, key2, buffout, outlen, enc, cbc)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key1, (unsigned char *)buffout, outlen, enc, cbc)

// 两个不同子密钥，加密，ECB模式
#define TriDes2KeysEE(buffin, inlen, key1, key2, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key1, (unsigned char *)buffout, outlen, 1, 0)

// 两个不同子密钥，解密，ECB模式
#define TriDes2KeysDE(buffin, inlen, key1, key2, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key1, (unsigned char *)buffout, outlen, 0, 0)

// 两个不同子密钥，加密，CBC模式
#define TriDes2KeysEC(buffin, inlen, key1, key2, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key1, (unsigned char *)buffout, outlen, 1, 1)

// 两个不同子密钥，解密，CBC模式
#define TriDes2KeysDC(buffin, inlen, key1, key2, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key1, (unsigned char *)buffout, outlen, 0, 1)


// 三个不同子密钥，加密，ECB模式
#define TriDes3KeysEE(buffin, inlen, key1, key2, key3, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key3, (unsigned char *)buffout, outlen, 1, 0)

// 三个不同子密钥，解密，ECB模式
#define TriDes3KeysDE(buffin, inlen, key1, key2, key3, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key3, (unsigned char *)buffout, outlen, 0, 0)

// 三个不同子密钥，加密，CBC模式
#define TriDes3KeysEC(buffin, inlen, key1, key2, key3, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key3, (unsigned char *)buffout, outlen, 1, 1)

// 三个不同子密钥，解密，CBC模式
#define TriDes3KeysDC(buffin, inlen, key1, key2, key3, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key3, (unsigned char *)buffout, outlen, 0, 1)

#endif
