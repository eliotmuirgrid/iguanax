// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBgrammar
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 07 November 2022 - 12:57PM
// ---------------------------------------------------------------------------

#include <DB/DBgrammar.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

DBcolumn::DBcolumn(const COLstring& iName, const COLstring& iDescription, DBsimpleType iType, bool iIsKey)
: Name(iName), Description(iDescription), Type(iType), IsKey(iIsKey) {
   COL_METHOD(DBcolumn::DBcolumn);
}

void DBcollection::clear(){
   COL_METHOD(DBcollection::clear);
   Tables.clear();
}