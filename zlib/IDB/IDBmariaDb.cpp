//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBmariaDb
//
// Description:
//
// Implementation
//
// Author: Julian Muir
// Date:   Monday, April 20th, 2021
//
//---------------------------------------------------------------------------
#include "IDBmariaDb.h"
#include "IDButils.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

void IDBmariaDb::setKnownMariaDbDlls(const COLarray<COLstring>& iKnownMariaDbDlls){
   KnownMariaDbDlls.clear();
   for ( COLarray<COLstring>::const_iterator DllIterator = iKnownMariaDbDlls.begin(); DllIterator != iKnownMariaDbDlls.end(); ++DllIterator){
      KnownMariaDbDlls.push_back(IDBfileCorrectFileName(*DllIterator));
   }
}

void IDBmariaDb::addKnownMariaDbDll(const COLstring& KnownDll){
   KnownMariaDbDlls.push_back(IDBfileCorrectFileName(KnownDll));
}

void IDBmariaDb::setPreferredMariaDbDll(const COLstring& iPreferredMariaDbDll){
   PreferredMariaDbDll = IDBfileCorrectFileName(iPreferredMariaDbDll);
}