#ifndef __HELP_UTILS_H__
#define __HELP_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HELPobject
//
// Description:
//
// Utility functions for Help model
//
// Author: Orkhan Amikishiyev
//---------------------------------------------------------------------------
class COLstring;
class COLvar;

void      HELPcontentToVar         (const COLstring& Content, COLvar* pData);
bool      HELPcheckPathOutOfBounds (const COLstring& Path, const COLstring& Component);
bool      HELPinvalidComponent     (const COLstring& Component);
COLstring HELPglobalDir();

#endif // end of defensive include