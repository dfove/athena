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

#include <stdio.h>
#include <stdlib.h>
#include <RMsgQueue.hpp>
#include <string>
#include <iostream>
#include <RStream.hpp>

using namespace athena;
using namespace std;

int main(int argc, char** argv)
{
	if ( ( argc != 3 && argc != 4 ) || ( strcmp(argv[1], "remove") != 0 && 
	       strcmp(argv[1], "send") != 0 && strcmp(argv[1], "recv") != 0  && 
	       strcmp(argv[1], "size") != 0)
	   )
    {
		cout << "Usage: " << argv[0] << " [remove|send|recv] [msgname] <msgtype>" << endl;
		return 0;
    }
	char buf[1024];
	memset(buf, 0, sizeof(buf));
	char sndbuf[] = "this is\0 the test!\n";
	cout << "******************************************" << endl;
    RMsgQueue msg(argv[2]);
//    RMsgQueue msg(atoi(argv[2]));
    if ( strcmp(argv[1], "size") == 0 )
    {
        cout << "msgqueue " << argv[2] << "'s size is " << msg.Size() << endl;
        return 0;
    }
    if ( strcmp(argv[1], "send") == 0 )
    {
	    if ( msg.Send(sndbuf, sizeof(sndbuf), atoi(argv[3])) == true )
	    //if ( msg.Send(sndbuf, 0, atoi(argv[3])) == true )
	    	cout << "msg send success " << endl;
		else
			cout << "msg send failure " << endl;
	}
	else if ( strcmp(argv[1], "recv") == 0 )
	{
	    memset(buf, 0, sizeof(buf));
	    if ( msg.TryRecv(buf, sizeof(buf), atoi(argv[3])) >= 0 )
	    	cout << "recv success, buf is " << buf << endl;
	    else
	    	cout << "recv failure! " << endl;
	}
	else	//remove
	{
		msg.Remove();
	}

	cout << "******************************************" << endl;
	RStream sendbuf("this is the RStream test"), recvbuf;
    if ( strcmp(argv[1], "send") == 0 )
    {
        if ( msg.Send(sendbuf, 100) == false )
            cout << "msg send error!" << endl;
        else
            cout << "msg send success!" << endl;
    }
	else if ( strcmp(argv[1], "recv") == 0 )
	{
	    if ( msg.TryRecv(recvbuf, 100) == -1 )
	        cout << "msg recv error!" << endl;
        else
            cout << "msg redv success, msg is: " << recvbuf.Data() << " size is " << recvbuf.Size() << endl;
	}

    return 0;
}
