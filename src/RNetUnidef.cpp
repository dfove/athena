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

#include <RNetUnidef.hpp>

namespace athena
{

int __startupWSock()
{
#ifdef WIN32
    int err;
    WSADATA wsdata;
    err = WSAStartup(MAKEWORD(2, 2), &wsdata);
    if (err != 0)
        return -1;
#endif
    return 0;
}

void __cleanupWSock()
{
#ifdef WIN32
    WSACleanup ();
#endif
}

#if defined(WIN32) || defined(_WIN32_WCE)

RString athena::GetSockError(int errnum)
{
    RString strerr;
	switch (errnum)
	{
	    case ERROR_TIMEOUT:
	        strerr.Format("ERROR_TIMEOUT: %d , %s\n", ERROR_TIMEOUT, "The timeout period expired. ");
	        break;
		case WSAEINTR :
		    strerr.Format("WSAEINTR: %d , %s\n", WSAEINTR, "Interrupted function call. ");
		    break;
		case WSAEACCES :
		    strerr.Format("WSAEACCES: %d , %s\n", WSAEACCES, "Permission denied. ");
		    break;
		case WSAEFAULT :
		    strerr.Format("WSAEFAULT: %d , %s\n", WSAEFAULT, "Bad address. ");
		    break;
		case WSAEINVAL :
		    strerr.Format("WSAEINVAL: %d , %s\n", WSAEINVAL, "Invalid argument. ");
		    break;
		case WSAEMFILE :
		    strerr.Format("WSAEMFILE: %d , %s\n", WSAEMFILE, "Too many open files. ");
		    break;
		case WSAEWOULDBLOCK :
		    strerr.Format("WSAEWOULDBLOCK: %d , %s\n", WSAEWOULDBLOCK, "Resource temporarily unavailable. ");
		    break;
		case WSAEINPROGRESS :
		    strerr.Format("WSAEINPROGRESS: %d , %s\n", WSAEINPROGRESS, "Operation now in progress. ");
		    break;
		case WSAEALREADY :
		    strerr.Format("WSAEALREADY: %d , %s\n", WSAEALREADY, "Operation already in progress. ");
		    break;
		case WSAENOTSOCK :
		    strerr.Format("WSAENOTSOCK: %d , %s\n", WSAENOTSOCK, "Socket operation on nonsocket. ");
		    break;
		case WSAEDESTADDRREQ :
		    strerr.Format("WSAEDESTADDRREQ: %d , %s\n", WSAEDESTADDRREQ, "Destination address required. ");
		    break;
		case WSAEMSGSIZE :
		    strerr.Format("WSAEMSGSIZE: %d , %s\n", WSAEMSGSIZE, "Message too long. ");
		    break;
		case WSAEPROTOTYPE :
		    strerr.Format("WSAEPROTOTYPE: %d , %s\n", WSAEPROTOTYPE, "Protocol wrong type for socket. ");
		    break;
		case WSAENOPROTOOPT :
		    strerr.Format("WSAENOPROTOOPT: %d , %s\n", WSAENOPROTOOPT, "Bad protocol option. ");
		    break;
		case WSAEPROTONOSUPPORT :
		    strerr.Format("WSAEPROTONOSUPPORT: %d , %s\n", WSAEPROTONOSUPPORT, "Protocol not supported. ");
		    break;
		case WSAESOCKTNOSUPPORT :
		    strerr.Format("WSAESOCKTNOSUPPORT: %d , %s\n", WSAESOCKTNOSUPPORT, "Socket type not supported. ");
		    break;
		case WSAEOPNOTSUPP :
		    strerr.Format("WSAEOPNOTSUPP: %d , %s\n", WSAEOPNOTSUPP, "Operation not supported. ");
		    break;
		case WSAEPFNOSUPPORT :
		    strerr.Format("WSAEPFNOSUPPORT: %d , %s\n", WSAEPFNOSUPPORT, "Protocol family not supported. ");
		    break;
		case WSAEAFNOSUPPORT :
		    strerr.Format("WSAEAFNOSUPPORT: %d , %s\n", WSAEAFNOSUPPORT, "Address family not supported by protocol family. ");
		    break;
		case WSAEADDRINUSE :
		    strerr.Format("WSAEADDRINUSE: %d , %s\n", WSAEADDRINUSE, "Address already in use. ");
		    break;
		case WSAEADDRNOTAVAIL :
		    strerr.Format("WSAEADDRNOTAVAIL: %d , %s\n", WSAEADDRNOTAVAIL, "Cannot assign requested address. ");
		    break;
		case WSAENETDOWN :
		    strerr.Format("WSAENETDOWN: %d , %s\n", WSAENETDOWN, "Network is down. ");
		    break;
		case WSAENETUNREACH :
		    strerr.Format("WSAENETUNREACH: %d , %s\n", WSAENETUNREACH, "Network is unreachable. ");
		    break;
		case WSAENETRESET :
		    strerr.Format("WSAENETRESET: %d , %s\n", WSAENETRESET, "Network dropped connection on reset. ");
		    break;
		case WSAECONNABORTED :
		    strerr.Format("WSAECONNABORTED: %d , %s\n", WSAECONNABORTED, "Software caused connection abort. ");
		    break;
		case WSAECONNRESET :
		    strerr.Format("WSAECONNRESET: %d , %s\n", WSAECONNRESET, "Connection reset by peer. ");
		    break;
		case WSAENOBUFS :
		    strerr.Format("WSAENOBUFS: %d , %s\n", WSAENOBUFS, "No buffer space available. ");
		    break;
		case WSAEISCONN :
		    strerr.Format("WSAEISCONN: %d , %s\n", WSAEISCONN, "Socket is already connected. ");
		    break;
		case WSAENOTCONN :
		    strerr.Format("WSAENOTCONN: %d , %s\n", WSAENOTCONN, "Socket is not connected. ");
		    break;
		case WSAESHUTDOWN :
		    strerr.Format("WSAESHUTDOWN: %d , %s\n", WSAESHUTDOWN, "Cannot send after socket shutdown. ");
		    break;
		case WSAETIMEDOUT :
		    strerr.Format("WSAETIMEDOUT: %d , %s\n", WSAETIMEDOUT, "Connection timed out. ");
		    break;
		case WSAECONNREFUSED :
		    strerr.Format("WSAECONNREFUSED: %d , %s\n", WSAECONNREFUSED, "Connection refused. ");
		    break;
		case WSAEHOSTDOWN :
		    strerr.Format("WSAEHOSTDOWN: %d , %s\n", WSAEHOSTDOWN, "Host is down. ");
		    break;
		case WSAEHOSTUNREACH :
		    strerr.Format("WSAEHOSTUNREACH: %d , %s\n", WSAEHOSTUNREACH, "No route to host. ");
		    break;
		case WSAEPROCLIM :
		    strerr.Format("WSAEPROCLIM: %d , %s\n", WSAEPROCLIM, "Too many processes. ");
		    break;
		case WSASYSNOTREADY :
		    strerr.Format("WSASYSNOTREADY: %d , %s\n", WSASYSNOTREADY, "Network subsystem is unavailable. ");
		    break;
	    case WSAVERNOTSUPPORTED:
		    strerr.Format("WSAVERNOTSUPPORTED: %d , %s\n", WSAVERNOTSUPPORTED, "Winsock.dll version out of range. ");
		    break;
	    case WSANOTINITIALISED:
		    strerr.Format("WSANOTINITIALISED: %d , %s\n", WSANOTINITIALISED, "Successful WSAStartup not yet performed. ");
		    break;
	    case WSAEDISCON:
		    strerr.Format("WSAEDISCON: %d , %s\n", WSAEDISCON, "Graceful shutdown in progress. ");
		    break;
	    case WSATYPE_NOT_FOUND:
		    strerr.Format("WSATYPE_NOT_FOUND: %d , %s\n", WSATYPE_NOT_FOUND, "Class type not found. ");
		    break;
	    case WSAHOST_NOT_FOUND:
		    strerr.Format("WSAHOST_NOT_FOUND: %d , %s\n", WSAHOST_NOT_FOUND, "Host not found. ");
		    break;
	    case WSATRY_AGAIN:
		    strerr.Format("WSATRY_AGAIN: %d , %s\n", WSATRY_AGAIN, "Nonauthoritative host not found. ");
		    break;
	    case WSANO_RECOVERY:
		    strerr.Format("WSANO_RECOVERY: %d , %s\n", WSANO_RECOVERY, "This is a nonrecoverable error. ");
		    break;
	    case WSANO_DATA:
		    strerr.Format("WSANO_DATA: %d , %s\n", WSANO_DATA, "Valid name, no data record of requested type. ");
		    break;
	    case WSA_INVALID_HANDLE:
		    strerr.Format("WSA_INVALID_HANDLE: %d , %s\n", WSA_INVALID_HANDLE, "Specified event object handle is invalid. ");
		    break;
	    case WSA_INVALID_PARAMETER:
		    strerr.Format("WSA_INVALID_PARAMETER: %d , %s\n", WSA_INVALID_PARAMETER, "One or more parameters are invalid. ");
		    break;
	    case WSA_IO_INCOMPLETE:
		    strerr.Format("WSA_IO_INCOMPLETE: %d , %s\n", WSA_IO_INCOMPLETE, "Overlapped I/O event object not in signaled state. ");
	    case WSA_IO_PENDING:
		    strerr.Format("WSA_IO_PENDING: %d , %s\n", WSA_IO_PENDING, "Overlapped operations will complete later. ");
	    case WSA_NOT_ENOUGH_MEMORY:
		    strerr.Format("WSA_NOT_ENOUGH_MEMORY: %d , %s\n", WSA_NOT_ENOUGH_MEMORY, "Insufficient memory available. ");
	    case WSA_OPERATION_ABORTED:
		    strerr.Format("WSA_OPERATION_ABORTED: %d , %s\n", WSA_OPERATION_ABORTED, "Overlapped operation aborted. ");
	    case WSASYSCALLFAILURE:
		    strerr.Format("WSASYSCALLFAILURE: %d , %s\n", WSASYSCALLFAILURE, "System call failure. ");
	}
	return strerr;
}

#endif      /*WIN32 || __WIN_CE*/

int 
GetByteOrder()
{
    typedef union 
    {
        short s;
        char  c[sizeof(short)];
    }un_t;
    un_t un;
    un.s = 0x0102;
    if ( sizeof(short) != 2 )
    {
        return -1;
    }
    if ( un.c[0] == 0x01 && un.c[1] == 0x02 )
    {
        return 1;   //big-endian byte order
    }
    else if ( un.c[0] == 0x02 && un.c[1] == 0x01 )
    {
        return 0;   //little-endian byte order
    }
    return -1;
}

}   //namespace
