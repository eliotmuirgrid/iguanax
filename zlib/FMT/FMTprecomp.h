#ifndef __FMT_PRECOMP_H__
#define __FMT_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file
//
// Author: Yunqi Zhang
// Date:   Oct 12, 2004
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#ifdef FMT_DEBUG
#   define FMT_LOG(Message) COLclog << Message << newline << ::flush;
#else
#   define FMT_LOG(Message)
#endif


#endif // end of defensive include
