#ifndef __PRECOMP_H__
#define __PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the install_shot Application.
//
// Author:   Kevin Senn
// Date:     Wednesday, September 14th, 2005 @ 10:39:15 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>



#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



// #define SHOT_DEBUG

#ifdef SHOT_DEBUG
#   define SHOT_LOG(Message) { COLcout << __FILE__ << ":" << __LINE__ << " " << Message << ::newline; }
#else
#   define SHOT_LOG(Message)
#endif

#endif // end of defensive include
