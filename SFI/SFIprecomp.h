#ifndef __SFI_PRECOMP_H__
#define __SFI_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the SFI library.
//
// Author:   Matt Lawrence
// Date:     Tuesday, July 25th, 2006 @ 03:51:06 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// Uncomment the next define to switch on error logging.  Please
// remember never to check this file into CVS with this define switched on.
//#define SFI_DEBUG

#ifdef SFI_DEBUG
#   define SFI_LOG(Message) { COLcout << __FILE__ << ':' << __LINE__ << ' ' << Message << ::newline; }
#else
#   define SFI_LOG(Message)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



#ifdef _WIN32
   #define _WIN32_WINNT 0x0400
   #define WIN32_LEAN_AND_MEAN
   #include <windows.h>
   #include <wincrypt.h>
#endif

#endif // end of defensive include
