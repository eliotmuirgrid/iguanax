#ifndef __COL_THREAD_H__
#define __COL_THREAD_H__
//-----------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  COLthread
//
// Description
//
//   Basic thread class.
//   Don't use COLthread directly - use COLthreadPool with COLclosure0.
//-----------------------------------------------------------------------------

class COLmutex;

typedef void* (*COLthreadFunction)(void*);

// MSYS2 works with long long, mingw on its own works with long
// #ifdef _WIN32
// typedef unsigned long COLthreadID;
// #else
typedef unsigned long long COLthreadID;
// #endif

// Get the thread ID of the OS thread we are in.
// Not related to COLthread instance.
// Not comparable with getThreadHandle() on Windows.
const COLthreadID COLthreadGetCurrentThreadID();

class COLthread
{
 public:
   // Create a thread instance but do not spawn a thread.
   // You must call start() to spawn a thread.
   COLthread();

   // Note: destructor does not join().
   // Instance must be in one of the following states:
   // 1. start() was never called
   // or 2. start() was called and then join()ed.
   ~COLthread();

   // Spawn a thread calling the function and arg provided.
   // May only call start() ONCE per instance.
   void start(COLthreadFunction function, void* arg);

   // Must be called from same thread start() was called.
   void join();

   // Get the thread handle of this COLthread instance
   // as seen at thread start().
   COLthreadID getThreadHandle() const;



   static void sleep(double seconds);
   static void yield();

   // treat as private
   static void* invokeThreadFunction(void* data);

 private:
   COLthread(const COLthread&);
   COLthread& operator=(const COLthread&);
   class COLthreadPrivate* pMember;
};

//-----------------------------------------------------------------------------
//
// COLcondition
//
// COLcondition::signal() and COLcondition::broadcast() are modelled
// after pthread_cond_signal and pthread_cond_broadcast.
// It is useful to read their man pages because their usage is often
// misunderstood.
//
// NOTE:
//
// Like pthread_cond_signal and pthread_cond_broadcast,
// COLcondition::signal() and COLcondition::broadcast() have
// NO EFFECT if there are no threads currently blocked on a wait().
//
// Because the condition can change before an awakened thread returns
// from COLcondition::wait(), the condition that caused the wait must be
// retested before the mutex lock is acquired. The recommended test
// method is to write the condition check as a while() loop that calls
// COLcondition::wait().
//
// Example:
//
//   COLmutex     CountMutex;
//   COLcondition CondVar;
//   int          Count = 0;
//
//   ...
//
//   void decrement_Count() {
//      COLlocker locker(CountMutex);
//      while (Count == 0) {
//         CondVar.wait(&CountMutex);
//      }
//      Count = Count - 1;
//   }
//
//   void increment_Count() {
//      COLlocker locker(CountMutex);
//      if (Count == 0) {
//         CondVar.signal();
//      }
//      Count = Count + 1;
//   }
//-----------------------------------------------------------------------------

class COLcondition{
 public:
   COLcondition();
   ~COLcondition();

   // Wait to be signalled
   void wait(COLmutex*);

   // Wake up one wait()er arbitrarily - no guarantee of fairness.
   void signal();

   // Wake up all wait()ers
   void broadcast();

 private:
   COLcondition(const COLcondition&);
   COLcondition& operator=(const COLcondition&);
   class COLconditionPrivate* pMember;
};

#endif // end of defensive include

