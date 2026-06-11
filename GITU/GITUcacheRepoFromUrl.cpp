//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITUcacheFromUrl.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   03/10/24 4:06 PM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <COL/COLlog.h>
#include <GITU/GITUcache.h>
#include <GITU/GITUcacheRepoFromUrl.h>
#include <GITU/GITUlist.h>
COL_LOG_MODULE;

bool GITUcacheRepoFromUrl(const COLstring& Username, const COLstring& GitUrl, const COLstring& Branch,
                          COLstring* pOutput) {
   COL_FUNCTION(GITUcacheRepoFromUrl);
   bool         Success;
   GITUlist     CloneList;
   GITUrepoInfo GitRepo;
   GitRepo.GitUrl = GitUrl;
   GitRepo.Branch = Branch;
   CloneList.List.push_back(GitRepo);
   GITUcloneList(Username, CloneList, pOutput, &Success);
   return Success;
}