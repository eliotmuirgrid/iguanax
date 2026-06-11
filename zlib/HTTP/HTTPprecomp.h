#ifndef __HTTP_PRECOMP_H__
#define __HTTP_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file for the HTTP library.
//
// Author:   Eliot Muir
// Date:     Thursday, December 7th, 2006 @ 03:20:33 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>



// Uncomment the next define to switch on error logging.  Please
// remember never to check this file into CVS with this define switched on.
//#define HTTP_DEBUG

#ifdef HTTP_DEBUG
#   define HTTP_LOG(Message) { COLcout << __FILE__ << ':' << __LINE__ << ' ' << Message << ::newline; }
#   define HTTP_PRECONDITION(Condition) COL_THROW_PRECONDITION(Condition)
#else
#   define HTTP_LOG(Message)
#   define HTTP_PRECONDITION(Condition)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <COL/COLerror.h>



#endif // end of defensive include
