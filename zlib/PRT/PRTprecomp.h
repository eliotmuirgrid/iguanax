#ifndef __PRT_PRECOMP_H__
#define __PRT_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the PRT library.
//
// Author:   Cheryl Chan
// Date:     Monday, March 12th, 2007 @ 10:36:28 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>



// Uncomment the next define to switch on error logging.  Please
// remember never to check this file into CVS with this define switched on.
//#define PRT_DEBUG

#ifdef PRT_DEBUG
#   define PRT_LOG(Message) { COLcout << __FILE__ << ':' << __LINE__ << ' ' << Message << ::newline; }
#else
#   define PRT_LOG(Message)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



#endif // end of defensive include
