//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBmySql
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Monday, April 5th, 2021 @ 02:30:42 PM
//
//---------------------------------------------------------------------------
#include "IDBmySql.h"
#include "IDButils.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

void IDBmySql::setKnownMySqlDlls(const COLarray<COLstring>& iKnownMySqlDlls){
   KnownMySqlDlls.clear();
   for ( COLarray<COLstring>::const_iterator DllIterator = iKnownMySqlDlls.begin(); DllIterator != iKnownMySqlDlls.end(); ++DllIterator){
      KnownMySqlDlls.push_back(IDBfileCorrectFileName(*DllIterator));
   }
}

void IDBmySql::addKnownMySqlDll(const COLstring& KnownDll){
   KnownMySqlDlls.push_back(IDBfileCorrectFileName(KnownDll));
}

void IDBmySql::setPreferredMySqlDll(const COLstring& iPreferredMySqlDll){
   PreferredMySqlDll = IDBfileCorrectFileName(iPreferredMySqlDll);
}