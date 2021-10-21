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

#include <RXml.hpp>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>

using namespace athena;
using namespace std;

int main()
{
    RXml xmlParser;
    xmlParser.ParseFile("test.xml");
    RString strDomainName;
    int ret = xmlParser.GetValue("///DomainName", strDomainName);
    cout << "domain name is " << strDomainName << ", ret is " << ret << endl;
    vector< RString > strPhoneNum1;
    ret = xmlParser.GetValue("//InteriorLine/PhoneNumber/Item", strPhoneNum1);
    cout << "ret is " << ret << endl;
    vector< RString >::iterator iter1;
    for ( iter1 = strPhoneNum1.begin(); iter1 != strPhoneNum1.end(); iter1++)
    {
        cout << *iter1 << endl;
    }

    struct stat statbuf;
    stat("test.xml", &statbuf);
    int nbytes = statbuf.st_size;
    int fd = open("test.xml", O_RDONLY);
    char* buffer = new char[nbytes + 1];
    int size = read(fd, buffer, nbytes);
    if ( size != nbytes )
    {
        cout << "read the file error" << endl;
        delete[] buffer;
        return 0;
    }
    buffer[nbytes] = '\0';

    cout << "################" << endl << buffer << endl << nbytes << " bytes" << endl << "################" << endl;

    RXml xmlBufParser;
    xmlBufParser.ParseMemory(buffer, nbytes);
    RString strDisplayName;
//    while (1)
//    {
        xmlBufParser.GetValue("/GatewayConfig/VoipConfig/ProxyAddress/Host", strDisplayName);
        cout << "display name is " << strDisplayName << endl;
        usleep(1000);
//    }


    RXml xmlBufParser2;
    xmlBufParser2.ParseMemory(buffer);
    vector< RString > strPhoneNum;
    xmlBufParser2.GetValue("//InteriorLine/PhoneNumber/Item", strPhoneNum);
    vector< RString >::iterator iter;
    for ( iter = strPhoneNum.begin(); iter != strPhoneNum.end(); iter++)
    {
        cout << *iter << endl;
    }

    delete[] buffer;
    return 0;
}
