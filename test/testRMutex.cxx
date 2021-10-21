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
#include <RMutex.hpp>
#include <RSMutex.hpp>
#include <RString.hpp>
#include <iostream>
#include <list>

using namespace std;
using namespace athena;

int main()
{
    RMutex mut;
    int i = 0;
    mut.Lock();
    //mut.Lock();
    i++;
    mut.Unlock();
    //mut.Unlock();
    printf("i is %d\n", i);

    RSafe< list<RString> > sflist;
    list<RString> tmp;
    tmp.push_back("test1");
    tmp.push_back("test2");
    printf("tmp list size is %ld\n", tmp.size());
    sflist.SetValue(tmp);
    printf("sflist list size is %ld\n", sflist.GetValue().size());

    RSafeEx< list<RString> > sfexlist;
    sfexlist.SetValue(tmp);
    printf("sfexlist list size is %ld\n", sfexlist.GetValue().size());
    return 0;
}
