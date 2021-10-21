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
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "Msg.hpp"

int main()
{
    CMsg mq(0x82569877);
    int ret = mq.GetMsg();
    printf("mq.GetMsg() return is %d\n", ret);
    MqBuf* msgbuf = NULL;
    while ( 1 )
    {
        int x = mq.TryRecv(&msgbuf);
        if ( x == -2 )
        {
            usleep(100);
            continue;
        }
        if ( x == -1 )
        {
            printf("receive message from mq error:%d, %s\n", errno, strerror(errno));
            break;
        }
        printf("receive message from mq success,type:%d, text:%s\n", msgbuf->mtype, msgbuf->mtext);
    }
    return 0;
}