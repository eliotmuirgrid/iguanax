//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: FSBvolumes
//
// Author: Matthew Nitsopoulos
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include "FSBvolumes.h"

#include <FIL/FILstorageManager.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;


void FSBfileBrowserVolumesImp(COLvar& Result) {
   COL_FUNCTION(FSBfileBrowserVolumesImp);
   FILstorageManager StorageManager;
   Result["volumes"].setArrayType();
#ifdef _WIN32
   for (int i = 0; i < StorageManager.countOfDevice(); i++) {
      Result["volumes"].push_back(StorageManager.deviceName(i));
   }
#endif
}

// /file_browser/volumes
void FSBfileBrowserVolumes(const COLwebRequest& Request) {
   COL_FUNCTION(FSBfileBrowserVolumes);
   COLvar Data = Request.Data;
   COL_VAR(Data);
   COLvar Result;
   FSBfileBrowserVolumesImp(Result);
   COL_VAR(Result.json(1));
   COLrespondSuccess(Request.Address, Result);
}