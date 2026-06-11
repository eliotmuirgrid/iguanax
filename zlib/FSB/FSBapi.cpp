//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: FSBapi
//
// Author: Shuo Hou
//
// Description:
//
// set up apis used by file browser
//---------------------------------------------------------------------------

#include "FSBapi.h"
#include "FSBlist.h"
#include "FSBvolumes.h"
#include "FSBnewFolder.h"
#include "FSBextractPath.h"
#include "FSBresolvePath.h"
#include "FSBsep.h"

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void FSBapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(FSBapi);
   ApiMap.add("/file_browser/list",          &FSBfileBrowserList);
   ApiMap.add("/file_browser/volumes",       &FSBfileBrowserVolumes);
   ApiMap.add("/file_browser/extract_path",  &FSBfileBrowserExtractPath);
   ApiMap.add("/file_browser/new_folder",    &FSBfileBrowserNewFolder);
   ApiMap.add("/file_browser/resolve_path",  &FSBfileBrowserResolvePath);
   ApiMap.add("/file_browser/sep",           &FSBfileBrowserSep);
}