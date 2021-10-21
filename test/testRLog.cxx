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

#include <RLog.hpp>
#include <RTools.hpp>
#include <RString.hpp>
#include <RLogThread.hpp>

#include <stdarg.h>

using namespace athena;

class myRRecordFile:public RRecordFile
{
public:
    myRRecordFile():RRecordFile(){}

    myRRecordFile(const myRRecordFile& myRecFile):RRecordFile(myRecFile){}

    virtual ~myRRecordFile(){}

    virtual void PreCreateFile(){printf("myRRecordFile PreCreateFile()\n");}

    virtual void AfterCloseFile(){printf("myRRecordFile AfterCloseFile()\n");}

    virtual RRecordFile* Clone() const{return new myRRecordFile(*this);}

};

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
//    RRecord rec("APP", "./", "", "log", false);
//    rec.FormatStr("%s:%d", "this is the test", 100);
//    rec.SaveToFile();
//    rec.PushToList();
//
//    RRecord rec2("APP", "./", "", "log", true);
//    rec2.FormatMem("this is the test!\n", 10);
//    rec2.SaveToFile();

// test RLog
    RRecord rec3("APP1", "./", "dzwl_", "log", false, 60, true);
    rec3.SetRecordFileProc(new myRRecordFile);
    rec3.FormatStr("%s_%04d", rec3.GenerateFileName().c_str(), 999);
    rec3.SaveToFile();

    RString xxx("this is the test");
    RString yyy = xxx^"leijianghua";
    rec3.FormatStr("%s", yyy.c_str());
    rec3.PushToList();

//    RRecordFileList& insFlist = RRecordFileList::Instance();
//    RRecordList& ins = RRecordList::Instance();
    RRecord rec4("APP2", "./", "dzwl_", "log", false, 60);   //每分钟输出一个文件
    rec4.SetRecordFileProc(new myRRecordFile);

    RLog log("DZWL", "./", PER_DAY);
    RLog::SetLogLevel(LOG_INFO);

    RWriteLogThread* pThread = new RWriteLogThread(2);
    pThread->Create(RThread::Thr_Detach | RThread::Thr_Globe);

    int i = 0;
    while( i++ < 1 )
    {
//        insFlist.CheckTimeToCloseFile();
        rec3.FormatStr("%s_%d", yyy.c_str(), i);
        rec3.PushToList();

        rec4.FormatStr("every one second to output this test:%d", i);
        rec4.PushToList();
/*
        log.Fatal("this is the log:%d", i);
        log.Error("this is the log:%d", i);
        log.Warn("this is the log:%d", i);
        log.Info("this is the log:%d", i);
        log.Debug(__FILE__, __LINE__, "this is the log:%d\n", i);
*/
        log.Write(FATAL, "this is the log:%d", i);
        log.Write(ERROR, "this is the log:%d", i);
        log.Write(WARN, "this is the log:%d", i);
        log.Write(INFO, "this is the log:%d", i);
        log.Write(DEBUG, "this is the log:%d", i);

//        ins.SaveAllToFile();
        RTime::Sleep_s(1);
    }
    RString strDel("RE,DT,0008,20100804085740,20100804085740,460022802083999,355469010004351,0,3D2D,RE,DT,0008,20100804085740,20100804085740,460022802083999,355469010004351,0,3D2D,RE,DT,0008,20100804085740,20100804085740,460022802083999,355469010004351,0,3D2D,RE,DT,0008,20100804085740,20100804085740,460022802083999,355469010004351,0,3D2D,RE,DT,0008,20100804085740,20100804085740,460022802083999,355469010004351,0,3D2D,");
    log.Log(INFO, "%s\n", strDel.c_str());

    Format("%s", strDel.c_str());

    return 0;
}
