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

#include <RTime.hpp>
#include <RString.hpp>
#include <RStream.hpp>
#include <RUdpSocket.hpp>
#include <iostream>

using namespace athena;
using namespace std;

int main(int argc, char** argv)
{
    bool server = false;
    char* dstip;
    RString strDstip;
    int port;
    if ( argc == 3 && strcmp(argv[1], "server") == 0 )      //server
    {
        server = true;
        port = atoi(argv[2]);
    }
    else if ( argc == 3 )
    {
        dstip = argv[1];
        strDstip = RString(dstip);
        port = atoi(argv[2]);
    }
    else
    {
        cout << "Usage:" << argv[0] << " [server port|ip port] " << endl;
        return 0;
    }
    __startupWSock();
    RUdpSocket fd;
    //fd.Create(AF_INET, SOCK_DGRAM);
    if ( server == true )
    {
        RString buf;
        int n = 1024;
        RNetAddress raddr;
        fd.Bind(port, NULL);
        while ( 1 )
        {
            int retrcv = fd.RecvFrom(buf, n, raddr, 5000);
            if ( retrcv > 0 )
                cout << "recvfrom " << raddr.GetIP() << "buf is " << buf << endl;
            else if ( retrcv == 0 )
                cout << "recvfrom " << raddr.GetIP() << "remote is shutdown" << endl;
            else
            {
                cout << "recvfrom error:" << fd.ReportError() << endl;
                break;
            }
        }
    }
    else
    {
        int retsnd = -1;
        char sndbuf[] = "this is the test!";
        if ( ( retsnd = fd.SendTo(sndbuf, (int)strlen(sndbuf), dstip, port) ) != strlen(sndbuf) ) 
        {
            cout << "send error, ret is " << retsnd << ", buf len is " << strlen(sndbuf) << endl;
        }
        else
            cout << "send " << strlen(sndbuf) << " bytes success" << endl;
    }
    __cleanupWSock();
    return 0;
}
