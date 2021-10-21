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
#include <fcntl.h>
#include <string.h>


#define FILE_MODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

class RBasicAppx
{
protected:
    RBasicAppx(int argc, char** argv){}
public:
    virtual int Execute(){return Start();}
    virtual int Start() = 0;
protected:
    static RBasicAppx* m_pApp;
};

class RBtsApp : public RBasicAppx
{
public:
    RBtsApp(int argc, char** argv):RBasicAppx(argc, argv)
    {
    }

    virtual int Start()
    {
        FILE* fd = fopen("/home/leijh/mylib/test.txt","a+");
        char test[] = "this is the test!\n";
        for (int i = 0; i < 10; i++)
        {
            fwrite(test, strlen(test), 1, fd);
        }
        fclose(fd);
        return 0;
    }
    
    static RBasicAppx* Instance(int argc, char** argv)
    {
        if ( m_pApp == NULL )
            m_pApp = new RBtsApp(argc, argv);
        return m_pApp;
    }
};

int main(int argc, char** argv)
{
    RBasicAppx* app = RBtsApp::Instance(argc,argv);
    app->Execute();
    return 0;
}
