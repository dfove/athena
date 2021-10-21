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
#include <RTime.hpp>
#include <time.h>

using namespace athena;

int main(int argc, char** argv)
{
    printf("time_t:%ld, RTime::Time:%ld\n", time(0), RTime::Time(NULL));
    RTime rt;
    rt.GetCurrentTime();
    printf("time_t:%ld, milisecond:%d\n", rt.GetTime_t(), rt.GetMillisecond());
    return 0;
}
