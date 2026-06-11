#ifndef __SIG_SIGNALLER3_H___
#define __SIG_SIGNALLER3_H___
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SIGsignaller3 (copied from COLsignal3.h).
//
// Description:
//
// Rather than having SIGsignaller define all the signals 1,2,3 this is
// an experiment to see if we can have less signal overhead by using
// SIGsignaller1.h, SIGsignaller2.h etc. include files - i.e. one per signal
// type.
//
// Author: Nasron Cheong
// Date:   Dec 2004
//
//---------------------------------------------------------------------------

#include "SIGsignallerBase3.h"

#define SIG_SIGNALLER_PARAM_COUNT 3
#define SIG_SIGNALLER_HEADER_PAYLOAD "SIGsignallerMheader.h"
#include "SIGsignallerM.h"
#undef SIG_SIGNALLER_HEADER_PAYLOAD
#undef SIG_SIGNALLER_PARAM_COUNT 

#endif // end of defensive include
