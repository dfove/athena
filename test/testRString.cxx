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

#include <iostream>
#include <stdarg.h>
#include <stdio.h>

#include "RString.hpp"

using namespace std;
using namespace athena;

void Format(const char *formatStr, ...)
{
    if ( formatStr == NULL )
        return;
    va_list ap;
    va_start(ap, formatStr);
    char *p = NULL;
    int n = vsnprintf(p, 0, formatStr, ap);
    printf("n is %d\n", n);
//    printf("n is %d, %s, strlen(p) %d\n", n, p, strlen(p));
    va_end(ap);
}

int main()
{
    RString str1("this is the test");

    RString strDbConn("tdr/tdr@DXTDR");
    size_t nPos1 = strDbConn.find_first_of("/");
    if ( nPos1 == RString::npos )
        cout << "cant find /" << endl;
    cout << strDbConn.substr(0,nPos1) << endl;
    size_t nPos2 = strDbConn.find_first_of("@");
    if ( nPos2 == RString::npos )
        cout << "cant find @" << endl;
    cout <<  strDbConn.substr(nPos1+1, nPos2 - nPos1 - 1) << endl;
    cout <<  strDbConn.substr(nPos2+1) << endl;

    cout << str1 << endl;

    RString strDel("RE,DT,0008,20100804085740,20100804085740,460022802083999,355469010004351,0,3D2D,");
    std::vector<RString> strV = strDel.Strtok(",");
    std::vector<RString>::iterator iter;
    for (iter = strV.begin(); iter != strV.end(); iter++)
        printf("%s\t", iter->c_str());
    cout << endl;

    RString strFormat;
    strFormat.Format("1 %s, 2 %s, 3 %s, 4 %s\n", strDel.c_str(), strDel.c_str(), strDel.c_str(), strDel.c_str());
    cout << strFormat << "size is " << strFormat.length() << endl;

    Format("%s", strDel.c_str());   //需要测试strDel超长的情况

    return 0;
}
