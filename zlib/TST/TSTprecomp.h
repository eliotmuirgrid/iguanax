#ifndef __TST_PRECOMP_H__
#define __TST_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the TST library.
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>



// Uncomment the next define to switch on error logging.  Please
// remember never to check this file into CVS with this define switched on.
//#define TST_DEBUG

#ifdef TST_DEBUG
#   define TST_LOG(Message) { COLcout << __FILE__ << ":" << __LINE__ << " " << Message << ::newline; }
#else
#   define TST_LOG(Message)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



#include "TSTtype.h"
#include "TSTexception.h"
#include "TSTtestResult.h"

#ifdef _WIN32
#   define VC_EXTRALEAN
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <winbase.h>
#endif

#endif // end of defensive include
