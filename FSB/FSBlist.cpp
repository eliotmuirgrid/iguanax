//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: FSBlist
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include "FSBlist.h"

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <MATCH/MATCHsearch.h>
COL_LOG_MODULE;

bool FSBisValidFileType(const COLstring& FileType, const COLstring& FilExt){
   COL_FUNCTION(FSBisValidFileType);
   return (!FileType.is_null() && (FileType == FilExt || FileType == "*.*" || FilExt == ""));
}

void FSBvalidFilesWithFilter(FILdirEnumerator& FileEnum, const COLstring& FileType, const COLstring& Filter,
                             COLvar* pList) {
   COL_FUNCTION(FSBvalidFilesWithFilter);
   COLstring FileName;
   while(FileEnum.getFile(FileName)) {
      COLstring FilExt = FILpathExtLast(FileName, false);
      COL_VAR2(FilExt, FileType);
      // Check if the file type is valid
      if(FSBisValidFileType(FileType, FilExt) && MATCHsearch(Filter, FileName, false)) {
         // Matches extension, allows all files, or is a folder
         (*pList)[FileName]["file"] = !FileEnum.isDirectory();
         (*pList)[FileName]["type"] = FilExt;
         COL_TRC("match " + FileName);
      }
      COL_VAR2(FilExt, FileName);
   }
}

void FSBfileBrowserListImp(COLstring Path, const COLstring& FileType, const COLstring& Filter, bool ShowHidden, const COLaddress& Address) {
   COL_FUNCTION(FSBfileBrowserListImp);
   COL_VAR3(Path, FileType, ShowHidden);
   COLvar List;
   FILcorrectPathSeparators(Path);
   COL_VAR(Path);
   Path = FILexpand(Path);
   COL_VAR(Path);
   FILdirEnumerator Enum(Path + FIL_ANYFILE, false);
   Enum.moveToFirst(); // pretty sure this is not required as the constructor does this for us...
   FSBvalidFilesWithFilter(Enum, FileType, Filter, &List);
   if(ShowHidden){
      FILdirEnumerator HiddenEnum(Path + ".*", false);
      FSBvalidFilesWithFilter(HiddenEnum, FileType, Filter, &List);
   }
   COL_VAR(List.json(1));            
   COLrespondSuccess(Address, List);
}

// "/file_browser/list"
void FSBfileBrowserList(const COLwebRequest& Request) {
   COL_FUNCTION(FSBfileBrowserList);
   COL_VAR(Request.Data);
   FSBfileBrowserListImp(COLvarGet(Request.Data, "dir", "~/"), COLvarGet(Request.Data, "type", ""),
                         COLvarGet(Request.Data, "filter", ""), COLvarGetBool(Request.Data, "show_hidden", true),
                         Request.Address);
}