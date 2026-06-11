//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: INSTapi.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   6/15/23 11:46 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <INST/INSTaccountExists.h>
#include <INST/INSTadmin.h>
#include <INST/INSTapi.h>
#include <INST/INSTconfig.h>
#include <INST/INSTcopy.h>
#include <INST/INSTdefaultPath.h>
#include <INST/INSTdeleteVersion.h>
#include <INST/INSTdowngrade.h>
#include <INST/INSTdownload.h>
#include <INST/INSTexists.h>
#include <INST/INSTheartbeat.h>
#include <INST/INSTrequestLatest.h>
#include <INST/INSTrequestSpecial.h>
#include <INST/INSTserviceExists.h>
#include <INST/INSTserviceInstall.h>
#include <INST/INSTservicePath.h>
#include <INST/INSTserviceRunning.h>
#include <INST/INSTserviceStart.h>
#include <INST/INSTserviceStop.h>
#include <INST/INSTserviceUninstall.h>
#include <INST/INSTstop.h>
#include <INST/INSTupgrade.h>
#include <INST/INSTupgradeAccess.h>
#include <INST/INSTupgradeCheckAuto.h>
#include <INST/INSTupgradeGetAuto.h>
#include <INST/INSTupgradeSetAuto.h>
#include <INST/INSTurl.h>
#include <INST/INSTuserListApi.h>
#include <INST/INSTworkingDir.h>
COL_LOG_MODULE;

void INSTapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap) {
   COL_FUNCTION(INSTapi);
   ApiMap.add("/install/account_exists",   &INSTaccountExists);
   ApiMap.add("/install/copy",             &INSTcopy);
   ApiMap.add("/install/default_path",     &INSTdefaultPath);
   ApiMap.add("/install/working_dir",      &INSTworkingDir);
   ApiMap.add("/install/exists",           &INSTexists);
   ApiMap.add("/install/server/install",   &INSTserviceInstall);
   ApiMap.add("/install/service/exists",   &INSTserviceExists);
   ApiMap.add("/install/service/start",    &INSTserviceStart);
   ApiMap.add("/install/service/stop",     &INSTserviceStop);
   ApiMap.add("/install/service/running",  &INSTserviceRunning);
   ApiMap.add("/install/service/path",     &INSTservicePath);
   ApiMap.add("/install/service/uninstall",&INSTserviceUninstall);
   ApiMap.add("/install/url",              &INSTurl);
   ApiMap.add("/install/admin/create",     &INSTadminCreate);
   ApiMap.add("/install/ensure/admin", &INSTensureAdmin);
   ApiMap.add("/install/user/list",        &INSTuserListApi);
   ApiMap.add("/install/config/get",       &INSTconfigGet);
   ApiMap.add("/install/config/set",       &INSTconfigSet);
   ApiMap.add("/install/heartbeat",        &INSTheartbeat);
}

void INSTapiAsync(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SDMserver* pServer, SCKloop* pLoop){
   COL_FUNCTION(INSTapiAsync);
   ApiMap.add("/install/stop", COLnewClosure1(&INSTstop, pServer, pLoop));
}

void INSTapiNormal(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(INSTapiNormal);
   ApiMap.add("/upgrade/access",           &INSTupgradeAccess);
   ApiMap.add("/upgrade/download",         &INSTupgradeDownload);
   ApiMap.add("/upgrade/request/latest",   &INSTupgradeRequestLatest);
   ApiMap.add("/upgrade/request/special",  &INSTupgradeRequestSpecial);
   ApiMap.add("/upgrade/download/special", &INSTupgradeDownloadSpecial);
   ApiMap.add("/downgrade/list",           &INSTdowngradeList);
   ApiMap.add("/downgrade/delete",         &INSTdeleteVersion);
   ApiMap.add("/upgrade/set_auto",         &INSTupgradeSetAuto);
   ApiMap.add("/upgrade/get_auto",         &INSTupgradeGetAuto);
   ApiMap.add("/upgrade/check_auto",       &INSTupgradeCheckAuto);
}

void INSTapiReplace(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, DBDworld* pWorld){
   COL_FUNCTION(INSTapiReplace);
   ApiMap.add("/upgrade/replace",          COLnewClosure1(&INSTupgradeReplace, pLoop, pWorld));
}
