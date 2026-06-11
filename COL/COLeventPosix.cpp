//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc. All Rights Reserved 
// 
// Module: COLevent
//
// Description:
//
// POSIX Implementation of a critical section object
//
// Author: Eliot Muir
// Date:   Wed 09/23/1998 
//---------------------------------------------------------------------------
#include "COLevent.h"
#include "COLerror.h"
#include "COLmutex.h"
#include "COLlocker.h"
#include "COLthread.h"  // for COLcondition
#include "COLlog.h"
COL_LOG_MODULE;

#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>


class COLeventPrivate{
public:
   COLeventPrivate();
   ~COLeventPrivate();

   void lock();
   void unlock();

   void setIsSignalled(int NewValue);
   int isSignalled() const;

   COLmutex CriticalSection;
   COLcondition       Condition;

   volatile int Sticky;
private:
   volatile int IsSignalled;
};

inline int COLeventPrivate::isSignalled() const { return IsSignalled; }

inline void COLeventPrivate::setIsSignalled(int NewValue) { IsSignalled = NewValue; }

void COLeventPrivate::lock() {
   COLuint32 ReturnCode = pthread_mutex_lock((pthread_mutex_t*)CriticalSection.handle());
   //COLcout << "lock" << newline;
   if (0 != ReturnCode) {
      COLstring Error = COLlastErrorAsString();
      COL_ABORT("pthread_mutex_lock failed " << Error);
   }
}

void COLeventPrivate::unlock() {
   COLuint32 ReturnCode = pthread_mutex_unlock((pthread_mutex_t*)CriticalSection.handle());
   //COLcout << "unlock" << newline;
   if (0 != ReturnCode) {
      COLstring Error = COLlastErrorAsString();
      COL_ABORT("pthread_mutex_lock failed " << Error);
   }
}

COLeventPrivate::COLeventPrivate() {
   setIsSignalled(false);
}

COLeventPrivate::~COLeventPrivate() {}

void COLevent::createEvent( bool iSticky ){
   pMember = new COLeventPrivate();
   pMember->Sticky = iSticky;
}

COLevent::~COLevent() {
   delete pMember;
}

void COLevent::wait() {
   pMember->lock();
   // wait for the signal predicate to change
   while (!pMember->isSignalled()) {
      // pMember->CriticalSection must be locked before this call...
      pMember->Condition.wait(&pMember->CriticalSection);

      // ...and pMember->CriticalSection is locked after this call.
   }
   // if the event is not sticky
   if (!pMember->Sticky) {
      pMember->setIsSignalled(false);
   }
   pMember->unlock();
}

void COLevent::signal() {
   pMember->lock();
   pMember->setIsSignalled(true);
   pMember->Condition.broadcast();
   pMember->unlock();
}

void COLevent::reset() { 
   if (!pMember->Sticky) {
      return;
   }
   pMember->lock(); 
   pMember->setIsSignalled(false);
   pMember->unlock();
   return;
}

