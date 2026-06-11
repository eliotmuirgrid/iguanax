#ifndef __COL_TRACKER_H___
#define __COL_TRACKER_H___
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLtracker
//
// Description: Implement this interface to use COLtrackable.
//
// TODO - this class, and its counterpart - COLtrackable - could be moved into a
// legacy library (aka, LEG) if/when their usage is removed from SFI
// (see #22561).
//
// Author: Nasron Cheong
// Date:   Sept 2004
//
//---------------------------------------------------------------------------

class COLtrackable;
class COLtracker
{
public:
   virtual void onTrackableDestroy(COLtrackable* pTrackable) = 0;
};

#endif // end of defensive include
