//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITUaddRemote.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   03/10/24 2:36 PM
//  ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GITU/GITUaddRemote.h>
#include <GIT/GITbranch.h>
#include <GITU/GITUconfig.h>
#include <INI/INIdata.h>
COL_LOG_MODULE;

// See IX-1593
bool GITUaddRemote(const COLstring& ComponentDir, const COLstring& RemoteUrl, COLstring* pError){
   COL_FUNCTION(GITUaddRemote);
   COLstring Branch = GITbranchCurrent(ComponentDir);
   COL_VAR(Branch);
   INIdata Config;
   COLstring Error;
   GITUconfigLoad(ComponentDir, &Config, pError);
   GITUconfigRemoteOriginUrlSet(&Config, RemoteUrl);
   GITUconfigRemoteOriginFetchRuleSet(&Config);
   // TODO - more thought required with dealing with branches.
   INIdataKeySet(&Config, "branch", Branch, "remote", "origin");
   INIdataKeySet(&Config, "branch", Branch, "merge", "refs/heads/" + Branch);
   GITUconfigSave(ComponentDir, Config);
   return true;
}
