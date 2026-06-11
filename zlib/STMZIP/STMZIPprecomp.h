#ifndef __STMZIP_PRECOMP_H__
#define __STMZIP_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the STMZIP library.
//
// Author:   Eliot Muir
// Date:     Monday, June 9th, 2008 @ 03:02:12 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// Uncomment the next define to switch on error logging.  Please
// remember never to check this file into CVS with this define switched on.
//#define STMZIP_DEBUG

#ifdef STMZIP_DEBUG
#   define STMZIP_LOG(Message) { COLcout << __FILE__ << ':' << __LINE__ << ' ' << Message << ::newline; }
#else
#   define STMZIP_LOG(Message)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



#endif // end of defensive include
