#ifndef __DB_DLL_H__
#define __DB_DLL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdll
//
// Description:
//
// Routine to find client libraries see IE-2299
//
// Author: Eliot Muir
// Date:   Mon 13 Jul 2020 15:44:10 EDT
//
//---------------------------------------------------------------------------

#include <COL/COLarray.h>

class COLstring;
class COLostream;

bool DBdllFind(const COLstring& Dir, const COLstring& Match, COLstring* pResult, COLostream* pOutStream);

bool DBsearchPaths(const COLstring& Environment, COLarray<COLstring>* pList, COLostream* pOutStream);

bool DBdllFindInDirList(const COLarray<COLstring>& List, const COLstring& Match, COLstring* pResult, COLostream* pOutStream);

bool DBdllFindLatestBrewOdbc(COLstring* pLibDir, COLostream* pOutStream);

#endif // end of defensive include
