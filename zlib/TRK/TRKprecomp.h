#ifndef __TRK_PRECOMP_H__
#define __TRK_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the TRK library.
//
// Author:   Eliot Muir
// Date:     Monday, January 23rd, 2006 @ 08:10:05 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>



// Uncomment the next define to switch on error logging.  Please
// remember never to check this file into CVS with this define switched on.
//#define TRK_DEBUG

#ifdef TRK_DEBUG
#   define TRK_LOG(Message) { COLcout << __FILE__ << ":" << __LINE__ << " " << Message << ::newline; }
#else
#   define TRK_LOG(Message)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



#endif // end of defensive include
