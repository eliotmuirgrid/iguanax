#ifndef __COL_TRACKABLE_H___
#define __COL_TRACKABLE_H___
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLtrackable
//
// Description: Classes may implement this to monitor the lifetime of a class, 
// using something that implements COLtracker.
//
// TODO - this class, and its counterpart - COLtracker - could be moved into a
// legacy library (aka, LEG) if/when their usage is removed from SFI
// (see #22561).
//
// Author: Nasron Cheong
// Date:   Sept 2004
//---------------------------------------------------------------------------

class COLtracker;
class COLtrackablePrivate;
class COLtrackable 
{
public:
   COLtrackable();
   virtual ~COLtrackable();

   void addTracker(COLtracker* pTracker);
   void removeTracker(COLtracker* pTracker);

private:
   COLtrackablePrivate* pMember;
   COLtrackable(const COLtrackable&);
   COLtrackable& operator=(const COLtrackable&);
};

#endif // end of defensive include
