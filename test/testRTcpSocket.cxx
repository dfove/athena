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
#include <RTime.hpp>
#include <RString.hpp>
#include <RStream.hpp>
#include <RTcpSocket.hpp>
#include <iostream>



using namespace athena;
using namespace std;

int main(int argc, char** argv)
{
    bool server = false;
    char* dstip = NULL;
    RString strDstip;
    int port;
    if ( argc == 4 && strcmp(argv[1], "server") == 0 )      //server
    {
        server = true;
        dstip = argv[2];
        strDstip = RString(dstip);
        port = atoi(argv[3]);
    }
    else if ( argc == 3 )
    {
        dstip = argv[1];
        strDstip = RString(dstip);
        port = atoi(argv[2]);
    }
    else
    {
        cout << "Usage:" << argv[0] << " [server ip port|ip port] " << endl;
        return 0;
    }
    __startupWSock();
    RTcpSocket fd;
    //fd.Create();
    int ret;
    if ( server == true )       //server
    {
        printf("%s\n", dstip);
        ret = fd.Bind(port, dstip);
        if ( ret == false )
        {
            cout << "Bind error:"<<fd.ReportError() << endl;
        }
        ret = fd.Listen(5);
        if ( ret != RTcpSocket::E_Success )
        {
            cout << "Listen error" << fd.ReportError() << endl;
        }
        RTcpSocket connfd;
        RString srcip;
        int srcport;
        if  ( fd.Accept(connfd, srcip, srcport) == RTcpSocket::E_Success )
        {
            cout << "this is the test!" << endl;
            char tmp[1024];
//            RString str;
//            RStream stream;
            memset(tmp, 0, sizeof(tmp));
            cout << "recv connection, from:" << srcip << ",port:" << srcport << endl;
//            int n = -1;
            //1.
            //n = connfd.RecvOnce(tmp, sizeof(tmp));
            //cout << "infomation len is " << n << ", content is " << tmp << endl;
            //2.
//            n = connfd.Recvn(str, 1000);
//            cout << "str len is " << str.size() << ", content is " << str << endl;
            //3.
            int iFlag = 0;
            while ( (ret = connfd.Recvn(tmp, sizeof(tmp))) == RTcpSocket::E_Success)
            {
                cout << "recv " << iFlag++ << ", " << sizeof(tmp) << " byte success" << endl;
//                cout << "stream len is " << stream.Size()<< endl;// << ", content is " << stream.Data() << endl;
            }
                cout << "recv 1000 byte error, ret is " << ret << ", error is " <<connfd.ReportError() << endl;
            //connfd.Recvn(tmp, 16);
            connfd.Close();
        }
        fd.Close();
    }
    else        //client
    {
/*
        char tmp[] = "this is the test!";
        RString str("this is the test2!");
        RStream stream("this\0 is the test3!", 19);
        int n = -1;
        ret = fd.Connect(dstip, port);
        if ( ret == false )
        {
            cout << fd.ReportError() << endl;
        }
        //1.
        //n = fd.SendOnce(tmp, sizeof(tmp));
        //2.
        n = fd.SendOnce(str);
        cout << "send " << n << "bytes" << endl;
        //3.
   #ifdef WIN32
        Sleep(1000*1);
   #else
        sleep(1);
   #endif
        int i = 0;
        while(i++ < 1000)
        {
            n = fd.SendOnce(stream);
            cout << "send " << n << "bytes" << endl;
            if ( n == -1 )
            {
                cout << "send error " << fd.ReportError() << endl;
                break;
            }
        }
        //.
        //fd.Sendn(tmp, 16);
        fd.Close();
*/

        ret = fd.Connect(dstip, port);
        if ( ret == false )
        {
            cout << fd.ReportError() << endl;
            cout << "connect error" << endl;
        }
        else
            cout << "connect success" << endl;
        char buf[1000];
//        memset(buf, 0, sizeof(buf));
//        short* sbuf = (short*)buf;
//        *sbuf = htons(7);
//        buf[2] = 0X01;
//        int tmpEid = htonl(16777218);
//        memcpy(buf+3, &tmpEid, 4);
        int iFlag = 0;
        while ( iFlag++ < 10000 )
        {
            cout << "this is the test" << endl; 
            int n = fd.Sendn(buf, sizeof(buf));
            if ( n == RTcpSocket::E_Success )
                cout << "send iFlag:" << iFlag <<" , "<< n << " bytes" << endl;
            else
            {
                cout << "send error! ret is " << n << "error is " << fd.ReportError() << endl;
            }
        }
/*
        n = fd.RecvOnce(buf, 4);
        cout << "recv " << n << " bytes," << (buf[3]==0?"success!":"failure!") << endl;

        char dbuf[31];
        memset(dbuf, 0, sizeof(dbuf));
        sbuf = (short*)dbuf;
        *sbuf = htons(31);
        dbuf[2] = 0X09;
        int* ibuf = (int*)(dbuf+3);
        int* tbuf = (int*)(dbuf+7);
        char* imsi = dbuf + 11;
        char* imei = dbuf + 19;
        char* eid = dbuf + 27;
        for ( int loop = 0; loop < 0; loop++ )
        {
            *ibuf = htonl(loop);
            *tbuf = htonl(time(0));
            memcpy(imsi, "12345678", 8);
            memcpy(imei, "12345678", 8);
            memcpy(eid, "1002", 4);
            n = fd.SendOnce(dbuf, 31);
            cout << "send " << n << " bytes" << endl;
        }

        RTime::Sleep_s(24*60*60);
        fd.Close();
        RTime::Sleep_s(10);
*/

        /*
        char buf[35];
        memset(buf, 0, sizeof(buf));
        short* ibuf = (short*)buf;
        *ibuf = htons(32);
        memcpy(buf+2, "8a8e8564234bf5f201234c133a091401", 32);
        int n = fd.SendOnce(buf, 34);
        cout << "send " << n << " bytes" << endl;

        n = fd.RecvOnce(buf, 34);
        cout << "recv " << n << " bytes, taskid size is " << ntohs(*(short*)buf) << ", task id is " << buf+2 << endl;
        RTime::Sleep_s(10);
        fd.Close();
        RTime::Sleep_s(10);
        */
    }
    __cleanupWSock();
    return 0;
}
