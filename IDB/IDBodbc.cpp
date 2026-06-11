//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBodbc
//
// Description:
//
// Implementation
//
// Author: Julian Muir
// Date:   Monday, April 20th, 2021
//
//---------------------------------------------------------------------------
#include "IDBodbc.h"
#include "IDButils.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

void IDBodbc::setKnownOdbcDlls(const COLarray<COLstring>& iKnownOdbcDlls){
   KnownOdbcDlls.clear();
   for ( COLarray<COLstring>::const_iterator DllIterator = iKnownOdbcDlls.begin(); DllIterator != iKnownOdbcDlls.end(); ++DllIterator){
      KnownOdbcDlls.push_back(IDBfileCorrectFileName(*DllIterator));
   }
}

void IDBodbc::addKnownOdbcDll(const COLstring& KnownDll){
   KnownOdbcDlls.push_back(IDBfileCorrectFileName(KnownDll));
}

void IDBodbc::setPreferredOdbcDll(const COLstring& iPreferredOdbcDll){
   PreferredOdbcDll = IDBfileCorrectFileName(iPreferredOdbcDll);
}