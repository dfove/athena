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
#include <string.h>
#include <RBasicApp.hpp>
#include <RTime.hpp>
#include <RString.hpp>

using namespace athena;

#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

#ifdef _WIN32
#define RBASICAPP RBasicAppx
#include "RbasicAppx.hpp"
#else
#define RBASICAPP RBasicApp
#endif
class RBtsApp : public RBASICAPP
{
protected:
    RBtsApp(int argc, char** argv):RBASICAPP(argc, argv)
    {
#ifdef _WIN32
        fd = fopen("test.txt", "a+");
#else
        fd = fopen("test.txt","a+");
#endif
    }

public:
    virtual ~RBtsApp()
    {
        fclose(fd);
    }

    virtual int Start()
    {
        RString test;
        std::vector<int> xxx;
        GetPidFromName("start");
        std::vector<int>::iterator it;
        for (it = xxx.begin(); it != xxx.end(); it++)
            printf("pid %d\n", *it);
        for (int i = 0; i < 10; i++)
        {
            test.Format("this is the test %d!\n", i+1);
            fwrite(test, strlen(test), 1, fd);
            fflush(fd);
            RTime::Sleep_s(1);
        }
        return 0;
    }

    void HandleReload()
    {
        char test[] = "this is the reload test!\n";
        fwrite(test, strlen(test), 1, fd);
        fflush(fd);
        RBASICAPP::HandleReload();
    }

    void HandleExit()
    {
        char test[] = "this is the exit test!\n";
        fwrite(test, strlen(test), 1, fd);
        fflush(fd);
        RBASICAPP::HandleExit();
    }

    static RBASICAPP* Instance(int argc, char** argv)
    {
        if ( m_pApp == NULL )
            m_pApp = new RBtsApp(argc, argv);
        return m_pApp;
    }

private:
    FILE* fd;
};

int main(int argc, char** argv)
{
    RBASICAPP* app = RBtsApp::Instance(argc,argv);
    app->Execute();
    return 0;
}
