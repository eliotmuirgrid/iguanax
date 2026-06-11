#ifndef __FIL_PRECOMP_H__
#define __FIL_PRECOMP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Precompiled header file
//
// Author: Eliot Muir
// Date:   Fri 06/08/1999 
//
//---------------------------------------------------------------------------

#include <COL/COLerror.h>

#ifndef _WIN32
#   include <dirent.h>
#   include <sys/stat.h>
#   include <sys/types.h>
#else
#   include <COL/COLwindows.h>
#endif


#endif // end of defensive include
