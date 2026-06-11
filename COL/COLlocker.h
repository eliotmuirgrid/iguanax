#ifndef __COL_LOCKER_H__
#define __COL_LOCKER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: COLlocker
//
// Description:
//
// RA II Locker class - designed to provide safe, exception-proof locking
// of mutexes.
// i.e. imagine we have class Foo which has a mutex member
// and we are in a method of class Foo called X....
// void Foo::X()
// {
//   COLlocker Lock(m_Mutex);
//   // The constructor of the the COLlocker object has locked the
//   // mutex
//   // :
//   // We do some stuff, an error is encountered and an exception is
//   // is thrown.
//   // Because the Lock object is always destroyed when we exit the 
//   // function we are guaranteed to always unlock the mutex
//   // as we exit.
// }
// Without COLlocker...
// void Foo::X()
// {
//    m_Mutex.lock();
//    // we do some stuff, an error is encountered and an exception is thrown
//    // so we *never* call the next line:
//    m_Mutex.unlock();
//    // oh bugger, the application freezes because the mutex is
//    // is locked and other threads which try and invoke the lock() method
//    // will hang indefinitely.
// }
// 
// Author: Eliot Muir
// Date:   Mon 08/09/1999 
//---------------------------------------------------------------------------

#include "COLmutex.h"

class COLlocker{
public:
   COLlocker(COLmutex& Mutex);
   ~COLlocker(); 
private:
   COLmutex* m_pMutex;
   COLlocker(const COLlocker& Orig);
   COLlocker& operator=(const COLlocker& Orig);
};

// inline all methods because call overhead is huge for this class

inline COLlocker::COLlocker(COLmutex& Mutex) : m_pMutex(&Mutex) {
   // No need to check for exceptions here - it never fails.
   m_pMutex->lock();
}

inline COLlocker::~COLlocker() {
   // No need to check for exceptions here - it never fails.
   m_pMutex->unlock();
}

//---------------------------------------------------------------------------

// COLunlocker does the opposite of COLlocker - TODO DELETE THIS!  Eliot 
//
// It releases the lock in the constructor and it acquires the lock in the
// destructor.
class COLunlocker{
public:
   COLunlocker(COLmutex& Mutex);
   ~COLunlocker(); 
private:
   COLmutex* m_pMutex;

   COLunlocker(const COLunlocker& Orig);
   COLunlocker& operator=(const COLunlocker& Orig);
};

// inline all methods because call overhead is huge for this class

inline COLunlocker::COLunlocker(COLmutex& Mutex) : m_pMutex(&Mutex) {
   // No need to check for exceptions here - it never fails.
   m_pMutex->unlock();
}

inline COLunlocker::~COLunlocker() {
   // No need to check for exceptions here - it never fails.
   m_pMutex->lock();
}

#endif // end of defensive include

