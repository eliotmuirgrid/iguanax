#ifndef __DB_GRAMMAR_LOAD_H__
#define __DB_GRAMMAR_LOAD_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Routine to load grammar from JSON input as would come from serialized JSON file
// on disc.
//
// Author:   Eliot Muir
// Date:     8th November 2022
//---------------------------------------------------------------------------
#include <DB/DBgrammar.h>
class COLstring;
class DBcollection;

DBsimpleType DBmapType(const COLstring& Type);
bool DBsmartLoad(const COLstring& Content, DBcollection* pOutput, COLstring* pError, bool* pLegacyWarning);
bool DBload(const COLstring& JsonContent, DBcollection* pOutput, COLstring* pError);

#endif // end of defensive include
