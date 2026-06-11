#ifndef __TSH_PRECOMP_H__
#define __TSH_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the TSH library.
//
// Author:   Dmitri Klimenko
// Date:     Friday, July 27th, 2007 @ 12:17:49 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>



// Uncomment the next define to switch on error logging.  Please
// remember never to check this file into CVS with this define switched on.
//#define TSH_DEBUG

#ifdef TSH_DEBUG
#   define TSH_LOG(Message) { COLcout << __FILE__ << ":" << __LINE__ << " " << Message << ::newline; }
#else
#   define TSH_LOG(Message)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



#endif // end of defensive include
