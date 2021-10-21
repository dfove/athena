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

#ifndef RBASICAPPX_HPP_HEADER_INCLUDED
#define RBASICAPPX_HPP_HEADER_INCLUDED

#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <signal.h>

class RBasicAppx
{
protected:
    RBasicAppx(int argc, char** argv){}
public:
    virtual ~RBasicAppx(){}
    virtual int Execute()
    {
        InitSignal();
        assert_papp();
        return Start();
    }
    virtual int Start() = 0;
    virtual void HandleReload(){}

    void InitSignal()
    {
    #if !defined(WIN32) && !defined(_WINDOWS)
        signal(SIGHUP, RecvSignal);
        signal(SIGPIPE,RecvSignal);
        signal(SIGPWR,RecvSignal);
        signal(SIGCHLD,RecvSignal);
        signal(SIGXFSZ,RecvSignal);
        signal(SIGALRM,RecvSignal);
        signal(SIGQUIT,RecvSignal);
    #endif
        signal(SIGTERM,RecvSignal);
        signal(SIGINT,RecvSignal);
        signal(SIGILL, RecvSignal);
    }

    static void RecvSignal(int sig)
    {
        RBasicAppx* pApp = RBasicAppx::m_pApp;
        assert(pApp);
        printf("RecvSignal:%d\n", sig);
        pApp->InitSignal();
    }

    static void assert_papp()
    {
        RBasicAppx* pApp = RBasicAppx::m_pApp;
        assert(pApp);
    }

protected:
    static RBasicAppx* m_pApp;
};

RBasicAppx* RBasicAppx::m_pApp = 0;


#endif /*RBASICAPPX_HPP_HEADER_INCLUDED*/
