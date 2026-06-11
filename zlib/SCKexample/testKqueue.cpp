//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testKqueue
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue 18 Nov 2014 10:23:15 EST
//---------------------------------------------------------------------------

#include "testKqueue.h"

#include <UNIT/UNITapp.h>

#include <COL/COLclosure.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <SCK/SCKsystem.h>

#ifdef __APPLE__ 

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>

typedef struct kevent SCKkevent;

class SCKkqueue{
public:
   void addReadCallback(int SocketHandle, COLclosure0* pCallback);
   void addWriteCallback(int SocketHandle, COLclosure0* pCallback);

   struct kevent* changeArray();

private:
   COLarray<SCKkevent> m_ChangeSet;
};

void SCKkqueue::addReadCallback(int SocketHandle, COLclosure0* pCallback){
   SCKkevent& NewEvent = m_ChangeSet.push_back(); 
   NewEvent.ident = SocketHandle;
   NewEvent.filter = EVFILT_READ;
   NewEvent.flags = EV_ADD | EV_CLEAR;
   NewEvent.fflags = 0;
   NewEvent.data = 0;
   NewEvent.udata = NULL;
}

void SCKkqueue::addWriteCallback(int SocketHandle, COLclosure0* pCallback){
   SCKkevent& NewEvent = m_ChangeSet.push_back();
   NewEvent.ident = SocketHandle;
   NewEvent.filter = EVFILT_WRITE;
   NewEvent.flags = EV_ADD;
   
}


struct kevent* SCKkqueue::changeArray(){
   COLarray<SCKkevent>::iterator i = m_ChangeSet.begin();
   return i;
}

static void test(){
   COL_FUNCTION(test);
}

// The kqueue interface takes an array of kqueue structs - it's convenient to be able
// to use COLarray to manage this array with resizing etc.  This verifies that we can do this.

static void testVector(){
   COL_FUNCTION(testVector);
   SCKkqueue Queue;
   const int ArraySize = 10;
   for (int i=0; i < ArraySize; i++){
      Queue.addReadCallback(i, COLnewClosure0(&test));
   }
   struct kevent* pChangeArray = Queue.changeArray();
   for (int i=0; i < ArraySize; i++){
      COL_VAR(pChangeArray[i].ident);
      UNIT_ASSERT_EQUALS(i, pChangeArray[i].ident);
   }
   struct timespec ts;
   ts.tv_sec = 10;
   ts.tv_nsec = 1000; // nano seconds.
  
}

void testKqueue(UNITapp* pApp){
   pApp->add("kqueue/testVector", &testVector);
}

#endif

