#ifndef __COL_AWAIT_H__
#define __COL_AWAIT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: COLawait
//
// Description:
//
// Making Asynchronous operations synchronous using mutex and condition
// variable. wait() and sync() must be called from different threads.
// For example,
//  - the main thread waits for async file IO on a worker thread
//  - a Lua worker thread waits for an operation to complete on the main thread
//  - a worker thread waits for operations on other worker threads
//
// static void ReadFileWorker(COLstring FilePath, COLawait* pAwait, COLstring* pContent) {
//    FILreadFile(FilePath, pContent);
//    pAwait->sync();
// }
// void main() {
//    COLthreadPool Pool;
//    Pool.start();
//    COLstring File("config.json");
//    COLstring Content;
//    COLawait  Await;
//    Pool.scheduleTask(COLnewClosure0(&ReadFileWorker, File, &Await, &Content));
//    Await.wait();
//    COLvar Json;
//    Json.parse(Content);
// }
//
// Author: John Q
// Date:   Tue 11 Oct 2022 9:32:55 EDT
//---------------------------------------------------------------------------
#include <COL/COLmutex.h>
#include <COL/COLthread.h>

class COLawait{
public:
   COLawait() = default;
   ~COLawait() = default;
   COLawait(const COLawait& Orig) = delete;
   COLawait& operator=(const COLawait& Orig) = delete;

   void wait();
   void sync();

private:
   bool         m_Waiting { false };
   bool         m_Synced  { false };   // in case sync() is called before wait()
   COLmutex     m_Mutex;
   COLcondition m_CV;
   COLthreadID  m_WaitThread;
};

#endif // end of defensive include
