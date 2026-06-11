#ifndef __ifware_h__
#define __ifware_h__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Helper routines to stream LUA stuff to COLostream
//
// Author:   Eliot Muir
// Date:     Wednesday, April 28th, 2010 @ 10:14:19 AM
//
//---------------------------------------------------------------------------

#include "lobject.h"
#include "llex.h"
#include "ltm.h"

class COLostream;
class COLstring;


COLostream& operator<<(COLostream& Stream, TString* pLuaSring);

COLostream& operator<<(COLostream& Stream, LexState* pState);

COLostream& operator<<(COLostream& Stream, Zio* pLuaStream);

COLostream& operator<<(COLostream& Stream, const TValue* pValue);
COLostream& operator<<(COLostream& Stream, const TValue& Value);

COLstring IFluaToken(int Token);

COLostream& operator<<(COLostream& Stream, TMS Event);


#endif // end of defensive include
