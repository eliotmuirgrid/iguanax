#ifndef __TBM_PRECOMP_H__
#define __TBM_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the TBM library.
//
// Author:   Eliot Muir
// Date:     Friday, February 9th, 2007 @ 07:10:20 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>



// Uncomment the next define to switch on error logging.  Please
// remember never to check this file into CVS with this define switched on.
//#define TBM_DEBUG

#ifdef TBM_DEBUG
#   define TBM_LOG(Message) { COLcout << Message << ::newline; }
#else
#   define TBM_LOG(Message)
#endif

//#define TBM_DEBUG_ENDIAN
#ifdef TBM_DEBUG_ENDIAN
#   define TBM_ENDIAN_LOG(Message) { COLcout << Message << ::newline; }
#else
#   define TBM_ENDIAN_LOG(Message)
#endif


#define TBM_ASSERTS_ON

#ifdef TBM_ASSERTS_ON
#   define TBMASSERT(Condition) COLPRECONDITION(Condition) 
#else
#   define TBMASSERT(Condition)  
#endif

#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



#endif // end of defensive include
