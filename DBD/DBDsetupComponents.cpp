// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDsetupComponents
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 21 September 2022 - 07:03PM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfigSave.h>
#include <CMPC/CMPCrunDir.h>
#include <COL/COLlog.h>
#include <DBD/DBDsetupComponents.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <TRNC/TRNCcleanup.h>
#include <TRNC/TRNCinit.h>
COL_LOG_MODULE;

static void DBDremoveExpiredRunDirs(const CFGmap& Map) {
   COL_FUNCTION(DBDremoveExpiredRunDirs);
   FILdirEnumerator DirEnum(FILpathAppend(DIRcomponentRunDir(""), FIL_ANYFILE));
   COLstring        Dir;
   while(DirEnum.getFile(Dir)) {
      const COLstring DirPath = DIRcomponentRunDir(Dir);
      if(!FILisDirectory(DirPath)) { continue; }
      COL_VAR(Dir);
      if(Map.ConfigMap.count(Dir) == 0) {
         COLstring RemoveError;
         COL_TRC("Removing expired run dir: " << DirPath);
         if(!FILremoveFullDirNewSafe(DirPath, &RemoveError)) { COL_ERR("Failed to remove run dir: " + RemoveError); }
      }
   }
}

static void DBDsetupRunDirs(CFGmap* pMap) {
   COL_FUNCTION(DBDsetupRunDirs);
   for(const auto& it : pMap->ConfigMap) {
      const COLstring& Component = it.first;
      const COLstring  RunDir    = DIRcomponentRunDir(Component);
      const COLstring& Commit    = it.second.CoreConfig.Commit;
      COL_VAR3(Component, RunDir, Commit);
      if(Commit.is_null() || Commit == "DEVELOPMENT") {
         COL_TRC("Skipping " << Component << " because it is a development component.");
         continue;
      }
      if(FILfileExists(RunDir) && !FILisDirectoryEmpty(RunDir)) {
         COL_TRC("Run dir for " << Component << " already exists and is not empty. Skipping.");
         continue;
      }
      COLstring RunError;
      if(!CMPCrunDir(Component, it.second.CoreConfig.RunComp, Commit, &RunError)) {
         COL_TRC("Error setting up run dir for " << Component << ": " << RunError << "\nSetting to development");
         try {
            pMap->ConfigMap[Component].CoreConfig.Commit = "";
            FILwriteFile(DIRconfigFile(Component), CFGconfigSerialize(pMap->ConfigMap[Component]).json(1));
         } catch(const COLerror& E) { COL_TRC("Failed to save config for " << Component << ": " << E.description()); }
      }
   }
   DBDremoveExpiredRunDirs(*pMap);
}

static void DBDcheckForInvalidConfigs(CFGmap* pMap, IWEBconfig* pWebConfig) {
   COL_FUNCTION(DBDcheckForInvalidConfigs);
   COLarray<COLstring> InvalidComponents;
   TRNCinit(pMap, pWebConfig, &InvalidComponents);
   if(InvalidComponents.size() > 0) {
      COL_TRC("Have invalid configuration files");
      COL_VAR(InvalidComponents.size());
      TRNCarchive(InvalidComponents, "Invalid configuration file");
      TRNCremoveComponentsFromMap(InvalidComponents, pMap);
   }
}

void DBDsetupComponents(CFGmap* pMap, IWEBconfig* pWebConfig) {
   COL_FUNCTION(DBDsetupComponents);
   DBDcheckForInvalidConfigs(pMap, pWebConfig);
   DBDsetupRunDirs(pMap);
}
