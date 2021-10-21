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

#ifndef UNIDEF_HPP_HEADER_INCLUDED
#define UNIDEF_HPP_HEADER_INCLUDED

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

namespace athena
{

#if defined(DLL_EXPORTS) && (defined(WIN32) || defined(_WIN32_WCE))
#define ATHENA_IM   __declspec( dllimport )
#define ATHENA_EX   __declspec( dllexport )
#else
#define ATHENA_IM
#define ATHENA_EX
#endif

#ifndef NULL
#define NULL  0
#endif

#define	Min(a,b)	((a) < (b) ? (a) : (b))
#define	Max(a,b)	((a) > (b) ? (a) : (b))

#ifdef __SUNOS__
#define POSIX_IPC_PREFIX "/"
#else
#define POSIX_IPC_PREFIX "/"
//#define POSIX_IPC_PREFIX "/tmp/"
#endif

#define	MAXBUFFSIZE	8192	/* buffer size for reads and writes */
#define	MAXPATHSIZE	256	    /* max path size */

#ifdef _DEBUG                                       //仅在明确的调试模式下才使用断言
#define RASSERT(x) assert(x)
#else
#define RASSERT(x)
#endif

}   /*namespace*/

#endif /* UNIDEF_HPP_HEADER_INCLUDED */
