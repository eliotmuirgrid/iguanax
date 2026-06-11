#ifndef __DIS_PRECOMP_H__
#define __DIS_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the DIS library.
//
// Author:   Leslie Chau
// Date:     Wednesday, June 25th, 2008 @ 10:47:43 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// Uncomment the next define to switch on error logging.  Please
// remember never to check this file into CVS with this define switched on.
//#define DIS_DEBUG

#ifdef DIS_DEBUG
#   define DIS_LOG(Message) { COLcout << __FILE__ << ':' << __LINE__ << ' ' << Message << ::newline; }
#else
#   define DIS_LOG(Message)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



#endif // end of defensive include
