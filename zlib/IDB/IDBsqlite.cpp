//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBsqlite
//
// Description:
//
// Implementation
//
// Author: Julian Muir
// Date:   Monday, April 26th, 2021
//
//---------------------------------------------------------------------------
#include "IDBsqlite.h"
#include "IDButils.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

void IDBsqlite::setKnownSqliteDlls(const COLarray<COLstring>& iKnownSqliteDlls){
   KnownSqliteDlls.clear();
   for ( COLarray<COLstring>::const_iterator DllIterator = iKnownSqliteDlls.begin(); DllIterator != iKnownSqliteDlls.end(); ++DllIterator){
      KnownSqliteDlls.push_back(IDBfileCorrectFileName(*DllIterator));
   }
}

void IDBsqlite::addKnownSqliteDll(const COLstring& KnownDll){
   KnownSqliteDlls.push_back(IDBfileCorrectFileName(KnownDll));
}

void IDBsqlite::setPreferredSqliteDll(const COLstring& iPreferredSqliteDll){
   PreferredSqliteDll = IDBfileCorrectFileName(iPreferredSqliteDll);
}