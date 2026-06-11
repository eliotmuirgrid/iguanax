#ifndef __DB_GRAMMAR_SAVE_H__
#define __DB_GRAMMAR_SAVE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Routine to save DB table grammar formatted for readability.
//
// Author:   Eliot Muir
// Date:     Mon  7 Nov 2022 15:28:17 EST
//---------------------------------------------------------------------------

class COLstring;
class DBcollection;

void DBsave(const DBcollection& Grammar, COLstring* pOutput);

#endif // end of defensive include
