//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINIset.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   21/05/25 12:10 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <GINI/GINIdefault.h>
#include <GINI/GINIignore.h>
#include <GINI/GINIsetKeyTracking.h>
#include <GIT/GITadd.h>
#include <GIT/GITcommit.h>
#include <GIT/GITremoveTracking.h>
COL_LOG_MODULE;

static COLarray<COLstring> GINIkeyPatterns() {
   COLarray<COLstring> Defaults;
   Defaults.push_back("**/*.key");
   Defaults.push_back("**/*.pem");
   Defaults.push_back("**/*.ppk");
   Defaults.push_back("**/id_rsa");
   Defaults.push_back("**/id_dsa");
   Defaults.push_back("**/id_ecdsa");
   Defaults.push_back("**/id_ed25519");
   Defaults.push_back("**/private_key");
   Defaults.push_back("**/privatekey");
   return Defaults;
}

static void GINItrackKeys(const COLstring& ConfigDir, const COLstring& User, const COLarray<COLstring>& Patterns,
                          COLstring* pErr) {
   COL_FUNCTION(GINItrackKeys);
   for(const auto& p : Patterns) { GITadd(ConfigDir, p, true, pErr); }
   GITcommit(User, ConfigDir, "", "Removed private key files from .gitignore", pErr);
   COL_VAR(*pErr);
}

static void GINIuntrackKeys(const COLstring& ConfigDir, const COLstring& User, const COLarray<COLstring>& Patterns,
                            COLstring* pErr) {
   COL_FUNCTION(GINIuntrackKeys);
   const COLstring RmCmd = "--ignore-unmatch ";
   for(const auto& p : Patterns) { GITremoveTracking(ConfigDir, RmCmd + p, pErr); }
   GITcommit(User, ConfigDir, "", "Added private key files to .gitignore", pErr);
}

bool GINIsetKeyTrackingImpl(const COLstring& User, const bool AddSection, COLarray<COLstring>* pIgnoreLines,
                            COLstring* pErr) {
   COL_FUNCTION(GINIsetKeyTrackingImpl);
   const COLstring            GitIgnoreFile = DIRconfigFile(".gitignore");
   const COLarray<COLstring> KeyPatterns   = GINIkeyPatterns();

   GINIprocessIgnore(pIgnoreLines, GINI_KEY_SECTION, AddSection, KeyPatterns);

   const bool Success = GINIsaveIgnore(GitIgnoreFile, *pIgnoreLines, pErr);
   COL_VAR2(Success, *pErr);
   if(!Success) return false;

   const auto ConfigDir = DIRconfigDir();
   AddSection ? GINIuntrackKeys(ConfigDir, User, KeyPatterns, pErr) : GINItrackKeys(ConfigDir, User, KeyPatterns, pErr);
   COL_VAR(*pErr);
   return GINIaddAndCommitIgnore(User, pErr);
}

// /instance/set_key_tracking
void GINIsetKeyTracking(const COLwebRequest& Request) {
   COL_FUNCTION(GINIset);
   const COLstring GitIgnoreFile  = DIRconfigFile(".gitignore");
   const bool      Ignore         = COLvarRequiredBool(Request.Data, "ignore");
   auto            GitIgnoreLines = GINIreadIgnoreLines(GitIgnoreFile);
   GINIcheckDefaultExists(&GitIgnoreLines);
   COLstring  Error;
   const bool Success = GINIsetKeyTrackingImpl(Request.User, Ignore, &GitIgnoreLines, &Error);
   Success ? COLrespondSuccess(Request.Address)
           : COLrespondError(Request.Address, "Error saving new ignore rules: " + Error);
}