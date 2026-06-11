//-----------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLthread
//-----------------------------------------------------------------------------
#include "COLthread.h"
#include "COLlocker.h"
#include "COLerror.h"
#include "COLlog.h"

// See COL_MODULE_COLthread and COL_MODULE_COLcondition trick below
// to give create different trace modules names in same .cpp file.
// COL_LOG_MODULE;

#ifdef _WIN32
#  include <windows.h>
#  include <process.h>
   typedef HANDLE        COLthread_t;
#else
#  include <pthread.h>
#  include <stdlib.h>
#  include <sys/time.h>
#  include <sys/select.h>
#  include <unistd.h>
#  include <sched.h>
   typedef pthread_t     COLthread_t;
#endif

// COLthreads are single use and go through these states in order
enum COLthreadState {
   kNOT_STARTED = 0,
   kSTARTING    = 1,
   kSTARTED     = 2,
   kFINISHED    = 3,
};

#ifdef COL_ENABLE_TRACING
#undef  COL_MODULE
#define COL_MODULE  COL_MODULE_COLthread
static char COL_MODULE_COLthread[] = "COLthread";
#endif

class COLthreadPrivate {
public:
   COLthreadPrivate() {
      COL_METHOD(COLthreadPrivate::COLthreadPrivate);
      State_ = kNOT_STARTED;
      ThreadFunction_ = 0;
      ThreadFunctionData_ = 0;
      memset(&ThreadHandle_, 0, sizeof(ThreadHandle_));
   }
   ~COLthreadPrivate() {
      COL_METHOD(COLthreadPrivate::~COLthreadPrivate);
   }
   COLthreadState    State_;
   COLmutex          StartMutex_;
   COLthread_t       ThreadHandle_; // handle is recorded at thread create time.
   COLthreadFunction ThreadFunction_;
   void*             ThreadFunctionData_;
private:
   COLthreadPrivate(const COLthreadPrivate&);
   COLthreadPrivate& operator=(const COLthreadPrivate&);
};

COLthread::COLthread()
{
   COL_METHOD(COLthread::COLthread);
   COL_STATIC_ASSERT(sizeof(COLthreadID) == sizeof(COLthread_t));
   pMember = new COLthreadPrivate;
}

COLthread::~COLthread()
{
   COL_METHOD(COLthread::~COLthread);
   COL_VAR2(pMember->State_, pMember->ThreadHandle_);
   COLASSERT(pMember->State_ == kNOT_STARTED || pMember->State_ == kFINISHED);
   delete pMember;
}

// static
void* COLthread::invokeThreadFunction(void* ThisThread)
{
   COL_FUNCTION(COLthread::invokeThreadFunction);
   COLthread* This = (COLthread*)ThisThread;
   COLASSERT(This);
   COLthreadPrivate* pMember = This->pMember;
   COLASSERT(pMember);
   const COLthreadID currentThreadID = COLthreadGetCurrentThreadID();
   COL_VAR(currentThreadID);
   {
      // #23841 wait for ThreadHandle_ to be initialized.
      COLlocker guard(pMember->StartMutex_);
   }
   COLASSERT(pMember->ThreadHandle_);
#ifndef _WIN32
   COLASSERT((COLthreadID)pMember->ThreadHandle_ == currentThreadID);
#endif
   return (*pMember->ThreadFunction_)(pMember->ThreadFunctionData_);
}

#ifdef _WIN32
static unsigned __stdcall COLwinThreadFunc(void* ThisThread)
{
   COL_FUNCTION(COLwinThreadFunc);
   COLthread::invokeThreadFunction(ThisThread);
   _endthreadex(0);
   return 0;
}
#endif

void COLthread::start(COLthreadFunction func, void* data)
{
   COL_METHOD(COLthread::start);
   COL_VAR2(pMember->State_, pMember->ThreadHandle_);

   switch (pMember->State_) {
   case kSTARTING:
      { COL_ERR("thread was already starting - no op."); }
      return;
   case kSTARTED:
      { COL_ERR("thread was previously started - no op."); }
      return;
   case kFINISHED:
      { COL_ERR("thread was previously started and finished - no op."); }
      return;
   default:
      break;
   }

   COLASSERT(pMember->State_ == kNOT_STARTED);
   COLASSERT(pMember->ThreadHandle_ == 0);
   COLASSERT(func);

   pMember->ThreadFunction_ = func;
   pMember->ThreadFunctionData_ = data;

   // XXX still must add thread size logic in here.

#ifdef _WIN32
   unsigned temp = 0;
   {
      COLlocker guard(pMember->StartMutex_); // #23841
      pMember->ThreadHandle_ = (HANDLE)_beginthreadex(0, 0, COLwinThreadFunc, (void*)this, 0, &temp);
   }
   COLPRECONDITION(pMember->ThreadHandle_ != (HANDLE)(-1L)); //XXX should check errno for failure reason
#else
   int threadCreateRC = 0;
   {
      COLlocker guard(pMember->StartMutex_); // #23841
      threadCreateRC = pthread_create(&pMember->ThreadHandle_, 0, &COLthread::invokeThreadFunction, this);
   }
   COL_VAR(threadCreateRC);
   COLPRECONDITION(threadCreateRC == 0);
#endif
   COLASSERT(pMember->ThreadHandle_);

   pMember->State_ = kSTARTED;
}

void COLthread::join()
{
   COL_METHOD(COLthread::join);
   COL_VAR2(pMember->State_, pMember->ThreadHandle_);
   COLASSERT(pMember->State_ == kSTARTING || pMember->State_ == kSTARTED);
   COLASSERT(pMember->ThreadHandle_);
#ifdef _WIN32
   WaitForSingleObject(pMember->ThreadHandle_, INFINITE);
#else
   void* threadReturn = 0;
   pthread_join(pMember->ThreadHandle_, &threadReturn);
#endif
   pMember->State_ = kFINISHED;
}

// static
void COLthread::yield()
{
   COL_FUNCTION(COLthread::yield);
#ifdef _WIN32
   Sleep(0);
#else
   sched_yield();
#endif
}

void COLthread::sleep(double seconds)
{
   COL_FUNCTION(COLthread::sleep);
   if (seconds < 0.0) {
      seconds = 0.0;
   }
#ifdef _WIN32
   DWORD ms = (DWORD)(seconds * 1000);
   Sleep(ms);
#else
   struct timeval tv;
   tv.tv_sec = (unsigned long long)seconds;
   tv.tv_usec = (unsigned long long)((seconds - tv.tv_sec) * 1000000);
   ::select(0, NULL, NULL, NULL, &tv);
#endif
}

COLthreadID COLthread::getThreadHandle() const
{
   return (COLthreadID)pMember->ThreadHandle_;
}

const COLthreadID COLthreadGetCurrentThreadID()
{
#ifdef _WIN32
   return (COLthreadID)GetCurrentThreadId();
#else
   // XXX warning - not 100% portable on POSIX
   // but works on the platforms we support.
   return (COLthreadID)pthread_self();
#endif
}

//-----------------------------------------------------------------------------

#ifdef COL_ENABLE_TRACINGS
#undef  COL_MODULE
#define COL_MODULE  COL_MODULE_COLcondition
static char COL_MODULE_COLcondition[] = "COLcondition";
#endif

class COLconditionPrivate {
public:
#ifdef _WIN32
   COLconditionPrivate() {
      COL_METHOD(COLconditionPrivate::COLconditionPrivate);
      Waiters_ = 0;
      Signallers_ = 0;
      Semaphore_ = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
      COL_VAR2(Semaphore_, &Mutex_);
      COLASSERT(Semaphore_);
   }
   ~COLconditionPrivate() {
      COL_METHOD(COLconditionPrivate::~COLconditionPrivate);
      CloseHandle(Semaphore_);
   }
   int            Waiters_;
   int            Signallers_;
   COLmutex       Mutex_;
   HANDLE         Semaphore_;
#else
   pthread_cond_t condition_;
#endif
};

COLcondition::COLcondition()
{
   COL_METHOD(COLcondition::COLcondition);
   pMember = new COLconditionPrivate;
#ifdef _WIN32
   // windows
#else
   // unix
   pthread_cond_init(&pMember->condition_, 0);
#endif
}

COLcondition::~COLcondition()
{
   COL_METHOD(COLcondition::~COLcondition);
#ifdef _WIN32
   // windows
#else
   // unix
   pthread_cond_destroy(&pMember->condition_);
#endif
   delete pMember;
}

void COLcondition::signal()
{
   COL_METHOD(COLcondition::signal);
#ifdef _WIN32
   int n = 0;
   {
      COLlocker locker(pMember->Mutex_);
      if (pMember->Waiters_ > 0) {
         n = 1;
         pMember->Signallers_ += n;
         pMember->Waiters_    -= n;
         COL_VAR2(pMember->Signallers_, pMember->Waiters_);
      } else {
         // agrees with pthread_cond_signal behaviour
         COL_TRC(this << " COLcondition::signal called but no waiters - no op");
         return;
      }
   }
   if (n > 0) {
      // no need to signal - no one was waiting
      COL_VAR2(pMember->Semaphore_, n);
      ReleaseSemaphore(pMember->Semaphore_, n, NULL);
   }
#else
   const int rc = pthread_cond_signal(&pMember->condition_);
   COL_VAR(rc);
   COLASSERT(0 == rc);
#endif
}

void COLcondition::broadcast()
{
   COL_METHOD(COLcondition::broadcast);
#ifdef _WIN32
   int n = 0;
   {
      COLlocker locker(pMember->Mutex_);
      if (pMember->Waiters_ > 0) {
         n = pMember->Waiters_;
         pMember->Signallers_ += n;
         pMember->Waiters_ = 0;
         COL_VAR2(pMember->Signallers_, pMember->Waiters_);
      } else {
         // agrees with pthread_cond_broadcast behaviour
         COL_TRC(this << "COLcondition::broadcast called but no waiters - no op");
         return;
      }
   }
   if (n > 0) {
      // no need to broadcast - no one was waiting
      COL_VAR2(pMember->Semaphore_, n);
      ReleaseSemaphore(pMember->Semaphore_, n, NULL);
   }
#else
   const int rc = pthread_cond_broadcast(&pMember->condition_);
   COL_VAR(rc);
   COLASSERT(0 == rc);
#endif
}

void COLcondition::wait(COLmutex* cvMutex)
{
   COL_METHOD(COLcondition::wait(COLmutex*));
#ifdef _WIN32
   {
      COLlocker locker(pMember->Mutex_);
      COLASSERT(pMember->Waiters_ >= 0);
      COLASSERT(pMember->Signallers_ >= 0);
      pMember->Waiters_++;
      COL_VAR(pMember->Waiters_);
   }
   COLunlocker unlocker(*cvMutex);
   for (;;) {
      unsigned long rc = WaitForSingleObject(pMember->Semaphore_, INFINITE);
      COL_VAR(rc);
      COLlocker locker(pMember->Mutex_);
      if (pMember->Signallers_ > 0) {
         pMember->Signallers_--;
         COL_VAR(pMember->Signallers_);
         return;
      } else if (WAIT_OBJECT_0 != rc) {
         // No signallers.
         if (pMember->Waiters_ > 0) {
            pMember->Waiters_--;
            COL_VAR(pMember->Waiters_);
         }
         return;
      }
   }
#else
   const int rc = pthread_cond_wait(&pMember->condition_, (pthread_mutex_t*)cvMutex->handle());
   COL_VAR(rc);
   COLASSERT(0 == rc);
#endif
}

