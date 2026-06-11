//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SIGsignalReceiver.
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   April 2013
//
//---------------------------------------------------------------------------

#include "SIGprecomp.h"
#pragma hdrstop

#include "SIGsignalReceiver.h"
#include "SIGsignallerVoid.h"

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

inline void SIGsignalReceiver::addSignaller(SIGsignallerVoid* pSignaller)
{
   addTracker(pSignaller);
}

inline void SIGsignalReceiver::removeSignaller(SIGsignallerVoid* pSignaller)
{
   removeTracker(pSignaller);
}
