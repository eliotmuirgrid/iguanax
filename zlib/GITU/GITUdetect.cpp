//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUdetect
//
// Description: 
//
// Implementation
//
// Author: Vismay Shah
// Date:   Fri July 09 2021
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <GITU/GITUdetect.h>
#include <GITU/GITUsshkeygenDetect.h>
#include <GIT/GITversion.h>
#include <LINK/LINKconstants.h>
#include <POP/POPcolor.h>
#include <PRO/PROwhich.h>
COL_LOG_MODULE;

bool GITUhasYum(){
   COL_FUNCTION(GITUhasYum);
   COLstring Path;
   bool HasYum = PROwhich(&Path, "yum");
   COL_VAR(HasYum);
   return HasYum;
}

void GITUgitNotInstalledLinux(COLostream& Out){
   COL_FUNCTION(GITUgitNotInstalledLinux);
   Out << "See: " << POPpop(LINKgitDetect) << newline;
   Out << "You need to install Git. To do this, copy and run these commands in your terminal:" << newline;
   if (GITUhasYum()){
      Out << "   sudo yum update -y >> install.log" << newline;
      Out << "   sudo yum install git -y >> install.log" << newline;
   } else {
      Out << "   sudo apt update -y >> install.log" << newline;
      Out << "   sudo apt install git -y >> install.log" << newline;
   }
   Out << "   " << POPiguanaCommandLine(true) << " --service install" << newline;
   Out << newline;
}

void GITUnotInstalled(COLostream& Out){
   COL_FUNCTION(GITUnotInstalled);
   #ifdef __linux__
   return GITUgitNotInstalledLinux(Out);
   #endif
   Out << "You need to install git to do this. " << newline
       << "See: " << POPpop(LINKgitInstall) << newline;
}

bool GITUinstalled(COLstring* pResult){
   COL_FUNCTION(GITUcheckGitInstall);
   return PROwhich(pResult, "git");
}

// TODO - no way to find GIT if not on the PATH.
// /git/check_install
void GITUcheckGitInstall(const COLwebRequest& Request) {
   COL_FUNCTION(GITUcheckGitInstall);
   COLstring GitPath, Version, VersionErr;
   GITversion(&Version, &VersionErr);
   
   COLvar Result;
   Result["installed"] = GITUinstalled(&GitPath);
   if (!VersionErr.is_null()){ 
      Result["version"] = VersionErr; 
   } else { 
      Result["version"] = Version;
   }
   
   Result["git_path"]       = GitPath;
   Result["sshkeygen_path"] = GITUsshkeygenDetect();
   Result["ssh_path"]       = GITUsshDetect();

   COL_VAR(Result.json(1));
   COLrespondSuccess(Request.Address, Result);
}
