// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTdowngrade
//
// Description: Implementation
//
// Author: Matthew Sobkowski
// Date:   Friday 25 August 2023 - 10:36PM
// ---------------------------------------------------------------------------
#include <INST/INSTdowngrade.h>
#include <INST/INSTdownload.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>

#include <PRO/PROexecute.h>
#include <PRO/PROlaunch.h>

#include <FIL/FILdirEnumerator.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLweb.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLvar INSTdowngradeListVersions(COLstring const& Dir){
   COL_FUNCTION(INSTdowngradeListVersions);
   COLvar DirContent;
   if(!FILfileExists(Dir)) { return DirContent; }
   DirContent.setArrayType();
   FILdirEnumerator Enum(Dir + "*", false, true);
   COLstring FileName;
   Enum.moveToFirst();
   while (Enum.getFile(FileName)) {
      if(FileName.find("_iguana")) {
         COL_VAR(FileName);
         DirContent.push_back(FileName);
      }
   }
   return DirContent;
}

void INSTdowngradeList(const COLwebRequest& Request) {
   COLvar List = INSTdowngradeListVersions(INSTdownloadDir());
   COLrespondSuccess(Request.Address, List);
}