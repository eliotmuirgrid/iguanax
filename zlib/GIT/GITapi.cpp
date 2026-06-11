// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 25 March 2023 - 12:58PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GIT/GITapi.h>
#include <GIT/GITcustomCommand.h>
#include <GIT/GITsystemPath.h>
#include <GIT/GITverifyComponentModules.h>
#include <GIT/GITverifyInstanceModules.h>
#include <GITU/GITUcommitPag.h>
#include <GITU/GITUdetect.h>
#include <GITU/GITUsetup.h>
#include <GITU/GITUuncommittedChanges.h>
COL_LOG_MODULE;

void GITapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop, CFGmap* pMap) {
   COL_FUNCTION(GITapi);
   ApiMap.add("/git/uncommitted_changes", COLnewClosure1(&GITUuncommittedChanges, pMap, pLoop));
}

// TODO - shift into GITW
void GITapiAsync(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(GITapiAsync);
   ApiMap.add("/git/check_install",            &GITUcheckGitInstall);
   ApiMap.add("/git/commit/pag",               &GITUcommitPag);
   ApiMap.add("/git/custom_command",           &GITcustomCommand);
   ApiMap.add("/git/install/status",           &GITUsetup);
   ApiMap.add("/system/path",                  &GITsystemPath);
   ApiMap.add("/git/verify_component_modules", &GITverifyComponentModules);
   ApiMap.add("/git/verify_instance_modules",  &GITverifyInstanceModules);

}
