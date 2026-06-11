//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLmutex
//
// Description:
//
// Implementation.  Renamed from COLcriticalSection.cpp.
//
// Author: Eliot Muir
// Date:   Wed 09/23/1998 
//---------------------------------------------------------------------------
#include "COLmutex.h"

#ifdef _WIN32
#include "COLwindows.h"

class COLmutexPrivate{
public:
   COLmutexPrivate() { ::InitializeCriticalSection(&CriticalSectionHandle);       } // Mostly void functions - no return code      
   ~COLmutexPrivate(){ ::DeleteCriticalSection(&CriticalSectionHandle);           } 
   bool tryLock()    { return !!::TryEnterCriticalSection(&CriticalSectionHandle);}
   void lock()       { ::EnterCriticalSection(&CriticalSectionHandle);            }  
   void unlock()     { ::LeaveCriticalSection(&CriticalSectionHandle);            } 
   void* handle()    { return (void*)&CriticalSectionHandle;                      }
   CRITICAL_SECTION CriticalSectionHandle;
};
#else  // POSIX

#include <pthread.h>
#include <stdio.h>
#include <errno.h>

class COLmutexPrivate{
public:
   COLmutexPrivate(){
      //No point checking error codes here - see ticket #10168
      pthread_mutexattr_t recursiveAttr;
      pthread_mutexattr_init(&recursiveAttr);
      pthread_mutexattr_settype(&recursiveAttr, PTHREAD_MUTEX_RECURSIVE);
      pthread_mutex_init(&Mutex, &recursiveAttr);
      pthread_mutexattr_destroy(&recursiveAttr);
   }

   ~COLmutexPrivate() {pthread_mutex_destroy(&Mutex); }

   bool tryLock() { return pthread_mutex_trylock(&Mutex) == 0;}
   void lock()    { pthread_mutex_lock(&Mutex);        }
   void unlock()  { pthread_mutex_unlock(&Mutex);      }
   
   void* handle(){ return (void*)&Mutex; }

private:
   pthread_mutex_t Mutex;
   COLmutexPrivate(const COLmutexPrivate& Orig);
   COLmutexPrivate& operator=(const COLmutexPrivate& Orig); 
};
#endif  // end of POSIX implementation.

COLmutex::COLmutex()     { pMember = new COLmutexPrivate(); }
COLmutex::~COLmutex()    { delete pMember;                  }
bool  COLmutex::tryLock(){ return pMember->tryLock();       }
void  COLmutex::lock()   { pMember->lock();                 }
void  COLmutex::unlock() { pMember->unlock();               }
void* COLmutex::handle() { return pMember->handle();        }

// In ticket #10168 - comment from Andrew Vajoczki 
// Mutexes created with PTHREAD_MUTEX_RECURSIVE will never return EDEADLK in 
// locking a single mutex by design (it's recursive).
//
// Multi-lock deadlock detection is not done in any pthread implementation that 
// I'm am aware of. It would be too computationally expensive to implement in a
// standard threading library, and would slow multithreaded applications down considerably.
//
// Checking the return code of pthread_mutex_init could be done, but it is unlikely to 
// ever fail because most major operating system's pthread libraries implement it in a
// way that doesn't require any memory allocation or operating system resources.
