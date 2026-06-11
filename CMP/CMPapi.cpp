//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPapi
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
//---------------------------------------------------------------------------
#include <CMP/CMPaddCustom.h>
#include <CMP/CMPaddRequire.h>
#include <CMP/CMPapi.h>
#include <CMP/CMPdirAdd.h>
#include <CMP/CMPdirContent.h>
#include <CMP/CMPdirDelete.h>
#include <CMP/CMPdirRename.h>
#include <CMP/CMPfileAdd.h>
#include <CMP/CMPfileContent.h>
#include <CMP/CMPfileDelete.h>
#include <CMP/CMPfileRename.h>
#include <CMP/CMPfileUpload.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

void CMPapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(CMPapi);
   ApiMap.add("/component/file",           &CMPfileContent);
   ApiMap.add("/component/file/add",       &CMPfileAdd);
   ApiMap.add("/component/file/delete",    &CMPfileDelete);
   ApiMap.add("/component/file/rename",    &CMPfileRename);
   ApiMap.add("/component/file/upload",    &CMPfileUpload);
   ApiMap.add("/component/dir",            &CMPdirContent);
   ApiMap.add("/component/dir/add",        &CMPdirAdd);
   ApiMap.add("/component/dir/delete",     &CMPdirDelete);
   ApiMap.add("/component/dir/rename",     &CMPdirRename);
   ApiMap.add("/component/add/require",    &CMPaddRequire);
   ApiMap.add("/component/add/custom",     &CMPaddCustom);
}
