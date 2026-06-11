//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLtrackable
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Sept 2004
//---------------------------------------------------------------------------

#include "COLtrackable.h"
#include "COLtracker.h"
#include "COLarray.h"
#include "COLlog.h"
COL_LOG_MODULE;

class COLtrackablePrivate
{
public:
   COLarray<COLtracker*> Trackers;
};

COLtrackable::COLtrackable()
{
   pMember = new COLtrackablePrivate;
}

COLtrackable::~COLtrackable()
{
   try
   {
      for (COLint32 TrackerIndex = 0; TrackerIndex < pMember->Trackers.size(); TrackerIndex++)
      {
         pMember->Trackers[TrackerIndex]->onTrackableDestroy(this);
      }   
      delete pMember;
   }
   catch (const COLerror& Error)
   {
      COL_VAR(Error);
   }
}

void COLtrackable::addTracker(COLtracker* pTracker)
{
   for (COLint32 TrackerIndex = 0; TrackerIndex < pMember->Trackers.size(); TrackerIndex++)
   {
      if (pMember->Trackers[TrackerIndex] == pTracker)
      {
         return;
      }
   }
   pMember->Trackers.push_back(pTracker);
}

void COLtrackable::removeTracker(COLtracker* pTracker)
{
   for (COLint32 TrackerIndex = 0; TrackerIndex < pMember->Trackers.size(); TrackerIndex++)
   {
      if (pMember->Trackers[TrackerIndex] == pTracker)
      {
         pMember->Trackers.remove(TrackerIndex);
         return;
      }
   }
}
