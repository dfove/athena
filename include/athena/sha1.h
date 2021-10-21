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

#ifndef _SHA1_H_
#define _SHA1_H_

#define SHA1_HASH_SIZE		20

/*
 *  SHA1
 *
 *  Description:
 *      利用FIPS PUB 180-1中描述的SHA1算法生成20个字节的消息摘要
 *
 *  Parameters:
 *      msgbuff: [in]
 *          输入数据，长度小于2^64 bits
 *      msglen: [in]
 *          输入数据的长度，单位是字节
 *      mdbuff：[out]
 *          输出数据，20字节的消息摘要
 *
 *  Returns:
 *      0　 成功
 *      >0  失败
 *
 */
int SHA1(const unsigned char* msgbuff, unsigned msglen, unsigned char mdbuff[SHA1_HASH_SIZE]);

#endif
