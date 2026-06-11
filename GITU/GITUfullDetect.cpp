// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUfullDetect
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 14 March 2023 - 04:48PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GITU/GITUconfigGlobal.h>
#include <GITU/GITUfullDetect.h>
#include <GITU/GITUsshkeygenDetect.h>
#include <INI/INIdata.h>
#include <PRO/PROexecute.h>
#include <PRO/PROwhich.h>
#include <WEB/WEBrequest.h>
COL_LOG_MODULE;

bool GITUfullDetect(const COLstring& Username, COLstring* pError){
   COL_FUNCTION(GITUfullDetect);
   COLstring GitPath;
   if (!PROwhich(&GitPath, "git")){
      *pError = "Could not locate git on the command line.";
      return false;
   } 
   if (GITUsshDetect().is_null()){
      *pError ="Could not locate ssh on the command line.";
      return false;
   }
   if (GITUsshkeygenDetect().is_null()){
      *pError = "Could not locate ssh-keygen on the command line.";
      return false;
   }
   
   INIdata Config;
   GITUconfigLoadUser(Username, &Config);
   if (GITUconfigUserName(Config).is_null()){
      *pError = "GIT credential user.name configuration needs to be set.";
      return false;
   }
   if (GITUconfigUserEmail(Config).is_null()){
      *pError = "GIT credential user.email configuration needs to be set.";
      return false;
   }
   *pError = "GIT is installed and configured.";
   return true;
}