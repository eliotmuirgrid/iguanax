#ifndef __TXTT_PRECOMP_H__
#define __TXTT_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the TXTT library.
//
// Author:   Kevin Senn
// Date:     Wednesday, April 29th, 2009 @ 11:59:29 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// Uncomment the next define to switch on error logging.  Please
// remember never to check this file into CVS with this define switched on.
//#define TXTT_DEBUG

#ifdef TXTT_DEBUG
#   define TXTT_LOG(Message) { COLcout << __FILE__ << ":" << __LINE__ << " " << Message << ::newline; }
#else
#   define TXTT_LOG(Message)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



#endif // end of defensive include
