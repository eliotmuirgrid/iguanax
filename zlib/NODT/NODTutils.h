#ifndef __NODT_UTILS_H__
#define __NODT_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODTutils
//
// Description:
//
// Utility functions for NODexample.
//
// Author: Kevin Senn
// Date:   Tuesday, November 9th, 2010 @ 03:34:02 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class NODplace;

NODplace& NODTgetNamedChildRepeat(NODplace& Place, const COLstring& ChildName, int RepeatIndex);

#endif // end of defensive include
