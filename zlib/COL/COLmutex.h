#ifndef __COL_MUTEX_H__
#define __COL_MUTEX_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: COLmutex
//
// Description:
//
// A mutex object - use in conjunction with COLlocker
// This is the one thread object that we have in the COL library because
// it is necessary to implement many of the objects with the COL library.
// For the rest of the threading objects you should look in the MT library.
//
// Renamed from COLcriticalSection.
//
// It's useful to read this over.
//
// Author: Eliot Muir
// Date:   Mon 08/09/1999 
//---------------------------------------------------------------------------

#include "COLminimumInclude.h"

class COLmutexPrivate;

class COLmutex{
public:
   COLmutex();
   ~COLmutex();

   //returns true if lock acquired - 
   //PLEASE DO NOT USE
   bool tryLock();
   
   void lock();
   void unlock();

   // pointer to the raw mutex structure or handle - implementation only!
   void* handle();  
private:
   COLmutexPrivate* pMember;

   COLmutex(const COLmutex& Orig);
   COLmutex& operator=(const COLmutex& Orig);
};

#endif // end of defensive include
