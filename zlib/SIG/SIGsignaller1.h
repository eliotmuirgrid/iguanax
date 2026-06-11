#ifndef __SIG_SIGNALLER1_H___
#define __SIG_SIGNALLER1_H___
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SIGsignaller1 (copied from COLsignal1.h).
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

#include "SIGsignallerBase1.h"

#define SIG_SIGNALLER_PARAM_COUNT 1
#define SIG_SIGNALLER_HEADER_PAYLOAD "SIGsignallerMheader.h"
#include "SIGsignallerM.h"
#undef SIG_SIGNALLER_HEADER_PAYLOAD
#undef SIG_SIGNALLER_PARAM_COUNT 

#endif // end of defensive include
