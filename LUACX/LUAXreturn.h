#ifndef __LUAX_RETURN_H__
#define __LUAX_RETURN_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAXreturn
//
// Description:
//
// Function to see if chunk returns.
//
// Author: Kevin Senn
// Date:   Tuesday, May 17th, 2011 @ 03:08:15 PM
//---------------------------------------------------------------------------

class COLstring;

// Note that this does not validate syntactic correctness of the code.
// If the code is not syntactically correct, the result is invalid.
bool LUAXdoesChunkReturn(const COLstring& LuaCode);

#endif // end of defensive include
