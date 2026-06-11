#ifndef __SIG_SIGNAL_RECEIVER_H___
#define __SIG_SIGNAL_RECEIVER_H___
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SIGsignalReceiver.
//
// Description: Receiver of a signal from SIGsignaller.  Note that this
// inherits from COLtrackable.  The signaller "tracks" the receivers
// that are "listening" to it, so that if the signal receiver goes out
// of scope, the signaller knows it.
//
// Author: Kevin Senn
// Date:   April 2013
//
//---------------------------------------------------------------------------

#include <COL/COLtrackable.h>

class SIGsignallerVoid;

class SIGsignalReceiver : public COLtrackable
{
public:
   SIGsignalReceiver() {}
   virtual ~SIGsignalReceiver() {}

   // Identical, but better-named, versions of addTracker() and removeTracker().
   inline void addSignaller(SIGsignallerVoid* pSignaller);
   inline void removeSignaller(SIGsignallerVoid* pSignaller);

private:
   SIGsignalReceiver(const SIGsignalReceiver&);
   SIGsignalReceiver& operator=(const SIGsignalReceiver&);
};

#endif // end of defensive include
