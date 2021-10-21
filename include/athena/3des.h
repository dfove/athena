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

#define DATA_DEC	0		// ���ܱ�־
#define DATA_ENC	1		// ���ܱ�־

#define MODE_ECB	0		// ECBģʽ
#define MODE_CBC	1		// CBCģʽ

/*----------------------------------------------------------------------------------------------
** ������
  buffin              [in]      ��������
  inlen               [in]      �������ݵĳ���
  key1, key2, key3    [in]      ����Կ��ÿ������Կ8���ֽ�
  buffout             [in]      ������������ɵ��ó������ռ䣬ECBģʽ���������������
  				���뻺������8���ֽڣ�CBCģʽ������������������뻺������16�ֽ�
  outlen              [in,out]  ����������ĳ��ȣ����Ȳ���ʱ����������ʧ�ܲ����ʵ����Ҫ�ĳ���
  enc                 [in]      1��ʾ���ܣ�0��ʾ����
  cbc                 [in]      1��ʾCBCģʽ��0��ʾECBģʽ

** ���أ�
  1    �ɹ�
  0    ʧ��
----------------------------------------------------------------------------------------------*/
int TripleDES(unsigned char* buffin, unsigned long inlen, unsigned char key1[8], 
		unsigned char key2[8], unsigned char key3[8], unsigned char* buffout, 
		unsigned long* outlen, int enc, int cbc) ;

// ֧��������ͬ������Կ
#define TriDes2Keys(buffin, inlen, key1, key2, buffout, outlen, enc, cbc)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key1, (unsigned char *)buffout, outlen, enc, cbc)

// ������ͬ����Կ�����ܣ�ECBģʽ
#define TriDes2KeysEE(buffin, inlen, key1, key2, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key1, (unsigned char *)buffout, outlen, 1, 0)

// ������ͬ����Կ�����ܣ�ECBģʽ
#define TriDes2KeysDE(buffin, inlen, key1, key2, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key1, (unsigned char *)buffout, outlen, 0, 0)

// ������ͬ����Կ�����ܣ�CBCģʽ
#define TriDes2KeysEC(buffin, inlen, key1, key2, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key1, (unsigned char *)buffout, outlen, 1, 1)

// ������ͬ����Կ�����ܣ�CBCģʽ
#define TriDes2KeysDC(buffin, inlen, key1, key2, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key1, (unsigned char *)buffout, outlen, 0, 1)


// ������ͬ����Կ�����ܣ�ECBģʽ
#define TriDes3KeysEE(buffin, inlen, key1, key2, key3, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key3, (unsigned char *)buffout, outlen, 1, 0)

// ������ͬ����Կ�����ܣ�ECBģʽ
#define TriDes3KeysDE(buffin, inlen, key1, key2, key3, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key3, (unsigned char *)buffout, outlen, 0, 0)

// ������ͬ����Կ�����ܣ�CBCģʽ
#define TriDes3KeysEC(buffin, inlen, key1, key2, key3, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key3, (unsigned char *)buffout, outlen, 1, 1)

// ������ͬ����Կ�����ܣ�CBCģʽ
#define TriDes3KeysDC(buffin, inlen, key1, key2, key3, buffout, outlen)	\
	TripleDES((unsigned char *)buffin, inlen, key1, key2, key3, (unsigned char *)buffout, outlen, 0, 1)

#endif
