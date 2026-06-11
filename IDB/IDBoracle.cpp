//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IDBoracle
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Monday, April 5th, 2021 @ 02:30:42 PM
//
//---------------------------------------------------------------------------
#include "IDBoracle.h"
#include "IDButils.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

void IDBoracle::setKnownOracleDlls(const COLarray<COLstring>& iKnownOracleDlls){
   KnownOracleDlls.clear();
   for ( COLarray<COLstring>::const_iterator DllIterator = iKnownOracleDlls.begin(); DllIterator != iKnownOracleDlls.end(); ++DllIterator){
      KnownOracleDlls.push_back(IDBfileCorrectFileName(*DllIterator));
   }
}

void IDBoracle::addKnownOracleDll(const COLstring& KnownDll){
   KnownOracleDlls.push_back(IDBfileCorrectFileName(KnownDll));
}

void IDBoracle::setPreferredOracleDll(const COLstring& iPreferredOracleDll){
   PreferredOracleDll = IDBfileCorrectFileName(iPreferredOracleDll);
}
