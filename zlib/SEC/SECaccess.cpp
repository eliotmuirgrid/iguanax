// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SECaccess
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski
// Date: 2024-11-28
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <SEC/SECaccess.h>
#include <SEC/SECpathParser.h>
COL_LOG_MODULE;

static COLarray<COLstring>* s_pAllowedDirs         = nullptr;
static bool*                 s_pEnableAllowedDirs   = nullptr;
static bool*                 s_pRestrictOsExecution = nullptr;

void SECfreePointers() {
   delete s_pAllowedDirs;
   delete s_pEnableAllowedDirs;
   delete s_pRestrictOsExecution;
}

void SECinit(const bool FreshInstall) {
   COL_FUNCTION(SECinit);
   s_pAllowedDirs         = new COLarray<COLstring>();
   s_pEnableAllowedDirs   = new bool(FreshInstall);
   s_pRestrictOsExecution = new bool(FreshInstall);
   COLstring Error;
   COLvar    SecurityVar;
   if(!SECload(&SecurityVar, &Error)) {
      COL_ERR("Error loading allowed directories: " << Error);
      return;
   }
   SECset(SecurityVar);
}

bool SECset(const COLvar& Var) {
   COL_FUNCTION(SECset);
   COL_VAR(Var);
   if(Var.isNull()) {
      s_pAllowedDirs->clear();
      return true;
   }
   if(!Var.isMap() || !Var["dirs"].isArray()) {
      COL_TRC("Var in invalid format");
      return false;
   }
   *s_pRestrictOsExecution = Var["restrict_execution"].asBool();
   *s_pEnableAllowedDirs   = Var.exists("enabled") ? Var["enabled"].asBool() : Var["enable_directories"].asBool();
   s_pAllowedDirs->clear();
   for(const auto& Dir : Var["dirs"].array()) {
      COL_VAR(Dir);
      s_pAllowedDirs->push_back(Dir);
   }
   return true;
}

bool SECload(COLvar* pOut, COLstring* pErr) {
   COL_FUNCTION(SECload);
   try {
      const COLstring OldFIle      = DIRconfigFile("settings/allowed_dirs.json");
      const COLstring SecurityFile = DIRfileConfigSecurity();
      if(FILfileExists(OldFIle)) { FILrenameFile(OldFIle, SecurityFile); }
      if(!FILfileExists(SecurityFile)) {
         COL_TRC("Security file does not exist -- saving");
         const COLvar SecVar = SECtoVar();
         return SECsave(SecVar, pErr);
      }
      COLstring Data;
      FILreadFile(SecurityFile, &Data);
      if(!pOut->parse(Data)) {
         *pErr = "Error parsing allowed directories";
         return false;
      }
      COL_VAR(pOut->json(1));
      return pOut->size() == 0 || pOut->isMap();
   } catch(const COLerror& E) {
      *pErr = "Error loading allowed directories: " + E.description();
      return false;
   }
}

bool SECsave(const COLvar& Var, COLstring* pErr) {
   COL_FUNCTION(SECsave);
   try {
      if(Var.size() > 0 && !Var.isMap()) {
         *pErr = "Var in invalid format";
         return false;
      }
      FILwriteFile(DIRfileConfigSecurity(), Var.json(1));
      return true;
   } catch(const COLerror& E) {
      *pErr = "Error saving allowed directories: " + E.description();
      return false;
   }
}

static bool SECcheck(const COLstring& Path, COLstring* pErr) {
   COL_FUNCTION(SECcheck);
   const COLstring WorkingDir = FILworkingDir();
   if(WorkingDir.is_null()) {
      COL_TRC("Working dir is null");
      return false;
   }
   if(Path.find(WorkingDir) == 0) {
      COL_TRC(Path + " found in working dir: " << WorkingDir);
      return true;
   }
   for(const auto& Dir : *s_pAllowedDirs) {
      COL_VAR(Dir);
      if(Path.find(Dir) == 0) {
         COL_TRC(Path << " is within " << Dir);
         return true;
      }
   }
   *pErr = Path + " does not reside within any allowed directories";
   return false;
}

static bool SEClooksLikePath(const COLstring& Str) {
   return Str.find('/') != npos || Str.find('\\') != npos || Str.find(':') != npos;
}

bool SECvalidate(const COLstring& Path, COLstring* pErr, const COLstring& Command) {
   COL_FUNCTION(SECvalidate);
   const COLstring ResolvedPath = FILpathResolveEnvironment(Path);
   COL_VAR2(Path, ResolvedPath);
   if(s_pEnableAllowedDirs && !*s_pEnableAllowedDirs) {
      COL_TRC("Ignoring sandbox rules");
      return true;
   }
   if (!s_pAllowedDirs) {
      COL_ABORT("SECinit(true) to the set up the sandbox functionality needs to be done.");
      return false; 
   }
   // to turn off the path parsing of the command replace below with: COLarray<COLstring> PathsToCheck;
   COLarray<COLstring> PathsToCheck = SECpathParse(Command);
   if(!ResolvedPath.is_null() && SEClooksLikePath(Path)) { PathsToCheck.push_back(ResolvedPath); }
   for(const auto& it : PathsToCheck) {
      const COLstring Resolved = FILpathResolveEnvironment(it);
      COL_VAR2(it, Resolved);
      if(!SECcheck(Resolved, pErr)) { return false; }
   }
   return true;
}

bool SECosExecuteRestricted() { return *s_pRestrictOsExecution; }

COLvar SECtoVar() {
   COL_FUNCTION(SECtoVar);
   COLvar Out;
   Out["restrict_execution"] = *s_pRestrictOsExecution;
   Out["enable_directories"] = *s_pEnableAllowedDirs;
   Out["dirs"].setArrayType();
   for(const auto& Dir : *s_pAllowedDirs) { Out["dirs"].push_back(Dir); }
   return Out;
}
