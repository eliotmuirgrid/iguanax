#ifndef __TXT_PRECOMP_H__
#define __TXT_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the TXT library.
//
// Author:   Kevin Senn
// Date:     Wednesday, April 29th, 2009 @ 11:33:58 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// Uncomment the next define to switch on error logging.  Please
// remember never to check this file into CVS with this define switched on.
//#define TXT_DEBUG

#ifdef TXT_DEBUG
#   define TXT_LOG(Message) { COLcout << __FILE__ << ':' << __LINE__ << ' ' << Message << ::newline; }
#else
#   define TXT_LOG(Message)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



#endif // end of defensive include
