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

#include <RSemaphore.hpp>
#include <RThread.hpp>

#if defined(_WIN32) || defined(_WINDOWS)
  #include <windows.h>
  #define SLEEP(val) Sleep(val*1000)
#else
  #include <unistd.h>
  #include <string.h>
  #define SLEEP(val) sleep(val)
#endif
#include <iostream>

using namespace std;
using namespace athena;

void* thd1(void* arg)
{
    RSemaphore* sem = (RSemaphore*)(arg);
    cout << "thread1 begin wait" << endl;
    if ( sem->Wait() == true ) 
        cout << "thread1 wait success" << endl;
    return NULL;
}

void* thd2(void* arg)
{
    RSemaphore* sem = (RSemaphore*)(arg);
    if ( sem->Post() == true )
        cout << "thread2 post success" << endl;
    else
        cout << "thread2 post failure" << endl;
    return NULL;
}

int main(int argc, char** argv)
{
    if ( argc != 1 && (( argc != 2 && argc != 3 ) || ( strcmp(argv[1], "wait") != 0 
                        && strcmp(argv[1], "post") != 0 && strcmp(argv[1], "remove") != 0 )) )
    {
        cout << "Usage: " << argv[0] << " wait/post <semaphoreName>" << endl;
        return 0;
    }

    RSemaphore* sem;
    if ( argc == 1 )
    {
        sem = new RSemaphore(0, 1);
        RThread th1("thread1"), th2("thread2"), th3("thread3");
        th1.Create(thd1, sem, 1);
        th2.Create(thd1, sem, 1);
        cout << "sleeping 5 seconds ..." << endl;
        SLEEP(5);
        th3.Create(thd2, sem, 1);
        th1.Release();
        th2.Release();
        th3.Release();
    }
    else 
    {
        if ( argc == 3)
            sem = new RSemaphore(argv[2], 0);
        else
            sem = new RSemaphore(NULL, 0);
        if ( strcmp(argv[1], "wait") == 0 )
        {
            cout << "wait name is " << sem->GetName().c_str() << " value is " << sem->GetValue() << endl;
            if ( sem->Wait() == true )
            {
        	    cout << "success " << endl;
            }
        }
        else if ( strcmp(argv[1], "post") == 0 )
        {
            if ( sem->Post() == true )
            {
                cout << "post name is " << sem->GetName().c_str() << " value is " << sem->GetValue() << endl;
        	    cout << "success " << endl;
            }
        }
        else if ( strcmp(argv[1], "remove") == 0 )
        {
            if (sem->Remove() == true)
            {
                cout << "remove name is " << sem->GetName().c_str() << " value is " << sem->GetValue() << endl;
        	    cout << "success " << endl;
            }
        }
    }
    delete sem;
    return 0;
}

