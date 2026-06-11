#ifndef __UTF_PRECOMP_H__
#define __UTF_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the UTF library.
//
// Author:   Dmitri Klimenko
// Date:     Tuesday, February 10th, 2009 @ 03:04:12 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// Uncomment the next define to switch on error logging.  Please
// remember never to check this file into CVS with this define switched on.
//#define UTF_DEBUG

#ifdef UTF_DEBUG
#   define UTF_LOG(Message) { COLcout << __FILE__ << ":" << __LINE__ << " " << Message << ::newline; }
#else
#   define UTF_LOG(Message)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



#endif // end of defensive include
