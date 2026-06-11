#ifndef __TSM_PRECOMP_H__
#define __TSM_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the TSM library.
//
// Author:   Eliot Muir
// Date:     Sunday, February 11th, 2007 @ 09:29:08 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>



// Uncomment the next define to switch on error logging.  Please
// remember never to check this file into CVS with this define switched on.
//#define TSM_DEBUG

#ifdef TSM_DEBUG
#   define TSM_LOG(Message) { COLcout << __FILE__ << ':' << __LINE__ << ' ' << Message << ::newline; }
#else
#   define TSM_LOG(Message)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



#endif // end of defensive include
