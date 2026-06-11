// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUremoveRemote
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 22 March 2023 - 07:22PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GIT/GITbranch.h>
#include <GITU/GITUconfig.h>
#include <GITU/GITUremoveRemote.h>
#include <INI/INIdata.h>
COL_LOG_MODULE;

COLstring GITUremoveRemote(const COLstring& RepoDir){
   COL_FUNCTION(GITUremoveRemote);
   COL_VAR(RepoDir);
   COLstring Branch = GITbranchCurrent(RepoDir);  // or should we use default branch....
   INIdata Config;
   COLstring Error;
   GITUconfigLoad(RepoDir, &Config, &Error);
   COLstring OldRemote = GITUconfigRemoteOriginUrl(Config);
   if (OldRemote.is_null()){
      OldRemote = "There was no remote repo for this component.";
   }
   GITUconfigRemoteClear(&Config, Branch);
   GITUconfigSave(RepoDir, Config);
   return OldRemote;
}
