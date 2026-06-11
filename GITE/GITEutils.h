#ifndef __GITE_UTILS_H__
#define __GITE_UTILS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEutils
//
// Description:
//
// Utility functions for GITE library.
//
// Author: Vismay Shah 
// Date:   Tuesday 09 April 2024 - 12:59PM
// ---------------------------------------------------------------------------
#include <COL/COLstring.h>
class COLvar;

void GITEremoveFileIfForce(bool Force, const COLstring& FileName);
bool GITEupdateOwnersFile(const COLstring& Username, COLvar* pResult, COLstring* pError);
bool GITEreadOwnersFile(const COLstring& FileName, COLstring* pContent, COLstring* pError);
bool GITElistOwners(const COLstring& Username, COLvar* pResult, COLstring* pError);
COLstring GITEencodeString(const COLstring& String);

#endif // end of defensive include
