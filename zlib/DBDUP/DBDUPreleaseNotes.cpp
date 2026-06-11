// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDUPreleaseNotes
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 17 August 2023 - 9:49AM
// ---------------------------------------------------------------------------

#include <DBDUP/DBDUPreleaseNotes.h>

#include <DIR/DIRurl.h>

#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>

#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /release/changelog
void DBDUPreleaseNotes(const COLwebRequest& Request) {
   COL_FUNCTION(DBDUPreleaseNotes);
   COL_VAR(Request.Data);
   COLstring Version = COLvarRequiredString(Request.Data, "version");
   COLvar Response;

   WEBrequest ChangelogRequest;
   ChangelogRequest.Url = DIRreleaseServer() + Version + "/changelog.html";
   WEBresponse Changelog;
   WEBrequestBlockingGet(ChangelogRequest, &Changelog);
   COL_VAR(Changelog);
   if (Changelog.ReturnCode != 200){
      COLrespondError(Request.Address, "No changelog found.");
   }
   Response["changelog"] = Changelog.Body;
   COLrespondSuccess(Request.Address, Response);
}
