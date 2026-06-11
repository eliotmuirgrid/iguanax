#ifndef __DB_PRECOMP_H__
#define __DB_PRECOMP_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBpreComp
//
// Description:
//
// Pre-compiled header file.
//
// Author: Henry Tran
// Date:   November 25, 2003 
//
//---------------------------------------------------------------------------

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  define VC_EXTRALEAN
#endif

#include <COL/COLerror.h>

//#define DB2_DEBUG

#ifdef DB2_DEBUG
#   define DB2_LOG(Message) { COLcout << __FILE__ << ':' << __LINE__ << ' ' << Message << ::newline; }
#else
#   define DB2_LOG(Message)
#endif

#endif // end of defensive include
