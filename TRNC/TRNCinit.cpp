//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNCinit
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue 12 Jul 2022 15:30:00 EDT
//---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <FIL/FILdirEnumerator.h>
#include <IWEB/IWEBconfig.h>
#include <TRNC/TRNCinit.h>
COL_LOG_MODULE;

// This happens on the main thread.
void TRNCinit(CFGmap* pMap, IWEBconfig* pWebConfig, COLarray<COLstring>* pInvalid) {
   COL_FUNCTION(TRNCinit);
   const COLstring  ConfigDirPath = DIRcompsDir();
   FILdirEnumerator Sniffy(ConfigDirPath + FIL_ANYFILE, true, true);
   COLstring        FileName;
   while(Sniffy.getFile(FileName)) {
      COL_VAR(FileName);
      if(!CFGmapAddComponentFromGuid(pMap, FileName, pWebConfig->uniqueId())) {
         COL_TRC("Component: " + FileName + " is invalid -- adding to cleanup vector");
         pInvalid->push_back(FileName);
      }
   }
   CFGremoveDeletedSources(pMap);
   COL_VAR(pInvalid->size());
}
