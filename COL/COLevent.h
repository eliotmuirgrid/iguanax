#ifndef __COL_EVENT_H__
#define __COL_EVENT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: COLevent
//
// Description:
//
// A event object 
//
// An event object allows one thread the wait on the event.  Another thread
// can signal this event and therefore wake up the first event.
//
// Under POSIX this class wraps up Conditional Variables (see the POSIX Threads
// book for a good introduction)
//
// An COLevent is kind of like a smart efficient Boolean variable that works
// nicely between different threads.  Consider the problems you have with using
// just a straight boolean variable between two threads.
//
// If one thread is waiting for the boolean variable to change then it has
// to continually poll the boolean variable until it changes.  This means
// it's using up CPU time doing it's polling and also the reaction to the boolean
// changing may not be instantaneous.
//
// COLevent solves this by allowing threads to go to sleep waiting on the
// COLevent to change - the operating system takes responsibility for waking
// up the thread when it is signaled.
//
// Author: Eliot Muir
// Date:   Thu 02/24/2000 
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLeventPrivate;

class COLevent
{
public:
   // Use MTstickyEvent rather than this constructor argument if you want a sticky event.
   COLevent() : pMember(NULL) { createEvent(false); }
   virtual ~COLevent();
   // same as above, but does not time out
   void wait();

   void signal();

private:
   friend class MTstickyEvent;
   // so that only MTstickyEvent can access it
   // reset() is now a member of MTstickyEvent. Please update your code.
   void reset();
   COLevent(bool Sticky) : pMember(NULL) { createEvent(Sticky); }
   
   void createEvent(bool Sticky);


   COLeventPrivate* pMember;
   
   COLevent(const COLevent& Orig);
   COLevent& operator=(const COLevent& Orig);
};

#endif // end of defensive include
