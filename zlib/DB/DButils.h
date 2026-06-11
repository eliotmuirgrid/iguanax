#ifndef __DB_UTILS_H__
#define __DB_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DButils
//
// Description:  
//
// Useful helper functions for various databases.
//
// Author: Guillaume Couture-Levesque
// Date:   July 1st, 2008
//
//---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>

bool DBstringIsAscii(const COLstring& String);

void DBbinaryToHex(COLsink& Sink, const void* pData, COLuint32 Size);

#endif // end of defensive include
