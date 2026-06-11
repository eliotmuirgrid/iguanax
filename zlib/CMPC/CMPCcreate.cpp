// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCcreate
//
// Description: Implementation
//
// Author: Matthew Sobkowski
// Date:   Wednesday 26 March 2025 - 10:31AM
// ---------------------------------------------------------------------------
#include <CFG/CFGconfig.h>
#include <CFG/CFGconfigSave.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <CMPC/CMPCcreate.h>
#include <CMPC/CMPCrunDir.h>
#include <CMPC/CMPCutils.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DRX/DRXutils.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GINS/GINSinitialize.h>
#include <GIT/GITlatest.h>
#include <GIT/GITpath.h>
#include <GIT/GITutils.h>
#include <GITU/GITUcacheRepoFromUrl.h>
#include <GITU/GITUrepository.h>
#include <GITU/GITUtemplate.h>
#include <GUID/GUIDid.h>
#include <IWEB/IWEBconfig.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>

#include <memory>
COL_LOG_MODULE;

static void CMPCcreateFinal() {
   COL_FUNCTION(CMPCcreateFinal);
}

static void CMPCcreateCallbackMain(const COLarray<CFGconfig> Components, const COLvar Result,
                                   COLauto<COLclosure2<COLarray<CFGconfig>, COLvar>> pCallback) {
   try {
      pCallback->run(Components, Result);
   } catch(const COLerror& Error) {
      COL_ERR("ERROR - MAJOR PROBLEM - CMPCcreateCallbackMain HANDLER THREW EXCEPTION" << newline << Error);
   }
}

static void CMPCcreateCallbackMain(int Index, COLvar Result, COLauto<COLclosure2<int, COLvar>> pCallback) {
   try {
      pCallback->run(Index, Result);
   } catch(const COLerror& Error) {
      COL_ERR("ERROR - MAJOR PROBLEM - CMPCcreateCallbackMain HANDLER THREW EXCEPTION" << newline << Error);
   }
}

struct CMPCcreateTracker {
   CMPCcreateTracker(const COLstring User, const int Count, const bool Plain)
       : m_User(User), m_Count(Count), m_Plain(Plain) {}
   COLstring            m_User;
   int                  m_Count       = 0;
   COLarray<CFGconfig> m_Components  = COLarray<CFGconfig>(m_Count);
   COLstring            m_Error       = "";
   COLstring            m_Warning     = "";
   bool                 m_Plain       = false;
   bool                 m_FailedEarly = false;
};

static bool CMPCsetupCompDir(const COLstring& ComponentId, const bool PlainFolder, COLstring* pGitUrl, COLstring* pCommitId,
                             COLvar* pOut) {
   COL_FUNCTION(CMPCsetupCompDir);
   const COLstring DevDir = DIRcomponentDevDir(ComponentId);
   COLstring       Error;
   if(PlainFolder) {
      GITcleanupAllGitDirs(DevDir);
      *pCommitId = "";
      *pGitUrl   = "";
      return true;
   }
   if(!GITlatest(DevDir, pCommitId, &Error)) {
      (*pOut)["error"]   = "Could not get latest commit from " + *pGitUrl + ". " + Error;
      (*pOut)["success"] = false;
      FILremoveFullDirNewSafe(DevDir, &Error);
      return false;
   }
   COL_TRC("set run");
   if(!CMPCrunDir(ComponentId, "", *pCommitId, &Error)) {
      (*pOut)["error"]   = "Could not create run directory for " + ComponentId + ". " + Error;
      (*pOut)["success"] = false;
      FILremoveFullDirNewSafe(DIRcomponentRunDir(ComponentId), &Error);
      return false;
   }
   if(!GITUrepository(DevDir, "origin", pGitUrl)) {
      COL_TRC("Could not get repository for " << ComponentId << ". Keeping given repository " << *pGitUrl);
   }
   return true;
}

static bool CMPCsetupBlank(const COLstring& RepoDir, bool PlainFolder, COLstring* pErr) {
   COL_FUNCTION(CMPCsetupBlank);
   const COLstring BlankCacheDir = GITUblankTemplateDir();
   try {
      FILcopyDirectoryContents(BlankCacheDir, RepoDir, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      if(PlainFolder) { GITcleanupAllGitDirs(RepoDir); }
      return true;
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      *pErr = E.description();
      return false;
   }
}

static void CMPCcheckMainLuaFile(const COLstring& Component, COLstring* pWarning) {
   COL_FUNCTION(CMPCcheckMainLuaFile);
   const COLstring DevDir       = DIRcomponentDevDir(Component);
   const COLstring MainFilePath = FILpathAppend(DevDir, COL_T("main.lua"));
   COL_VAR(MainFilePath);
   if(!FILfileExists(MainFilePath)) {
      COL_TRC("main.lua file does not exist");
      *pWarning = "The component may not run as expected, 'main.lua' file not found. Try doing 'Refresh Cache' in the component add screen if unexpected.";
   }
}

static bool CMPCsetupRepo(const COLstring& ComponentId, bool PlainFolder, COLstring& GitUrl, const COLstring& Branch, COLstring* pCommitId,
                   COLstring* pError) {
   COL_FUNCTION(CMPCsetupRepo);
   COL_VAR3(ComponentId, GitUrl, Branch);
   const COLstring DevDir = DIRcomponentDevDir(ComponentId);
   if(GitUrl.is_null()) {  // If no GitUrl is provided, set up a blank component
      if(CMPCsetupBlank(DevDir, PlainFolder, pError)) { return true; }
      *pError = "Could not setup blank component " + ComponentId + ". " + *pError;
      return false;
   }
   if(!GITUtemplateCreate(ComponentId, GitUrl, Branch, pError)) {  // If GitUrl provided, create a repository from the template
      *pError = "Could not create " + ComponentId + ". " + *pError;
      return false;
   }
   COLvar TempOut;
   if(!CMPCsetupCompDir(ComponentId, PlainFolder, &GitUrl, pCommitId, &TempOut)) {
      *pError = TempOut["error"].asString();
      return false;
   }
   return true;
}

static void CMPCcreateInstanceAdd(const COLarray<CFGconfig> Components, SCKloop* pMainLoop, COLclosure0* pCallback) {
   COL_FUNCTION(CMPCcreateInstanceAdd);
   COL_TRC("Adding to instance");
   COLarray<COLstring> ComponentGuids;
   for(const auto& it : Components) { ComponentGuids.push_back(it.CoreConfig.Guid); }
   GINSaddComponentList(ComponentGuids);
   pMainLoop->post(pCallback);
}

static void CMPCcreateUpdateMap(const COLarray<CFGconfig>& Components, CFGmap* pMap, BLUAinstanceManager* pManager,
                                SCKloop* pMainLoop, COLvar* pComponents) {
   COL_FUNCTION(CMPCcreateUpdateMap);
   pComponents->setArrayType();
   for(const auto& Comp : Components) {
      pComponents->push_back(CFGconfigSerialize(Comp));
      SDBIcreateQueue(Comp.CoreConfig.Guid);
      if(Comp.CoreConfig.Sources.size() > 0) {
         COL_TRC("Creating queue consumer from source: " << Comp.CoreConfig.Sources[0] << " to "
                                                         << Comp.CoreConfig.Guid);
         SDBIcreateQueueConsumer(Comp.CoreConfig.Sources[0], Comp.CoreConfig.Guid);
      }
      CFGmapAddComponent(pMap, Comp);
      CMPCstartComponentWithRun(Comp.CoreConfig.Guid, pManager, pMainLoop, pMap);
   }
   CFGremoveDeletedSources(pMap);
}

static void CMPCcreateFinish(const COLarray<CFGconfig>& Components, const COLstring& Warning, const COLstring& Error,
                             CFGmap* pMap, BLUAinstanceManager* pManager, SCKloop* pMainLoop,
                             const COLaddress Address) {
   COL_FUNCTION(CMPCcreateFinish);
   COLvar Response;
   if(!Warning.is_null()) { Response["warning"] = Warning; }
   CMPCcreateUpdateMap(Components, pMap, pManager, pMainLoop, &Response["components"]);
   Error.is_null() ? COLrespondSuccess(Address, Response) : COLrespondError(Address, Error);
}

static void CMPCsortFailedIndexesAscending(COLarray<int>* pFailedIndexes) {
   COL_FUNCTION(CMPCsortFailedIndexesAscending);
   const int size = pFailedIndexes->size();
   if(size <= 1) return;
   bool swapped;
   for(int i = 0; i < size - 1; i++) {
      swapped = false;
      for(int j = 0; j < size - i - 1; j++) {
         if((*pFailedIndexes)[j] < (*pFailedIndexes)[j + 1]) {
            int temp                 = (*pFailedIndexes)[j];
            (*pFailedIndexes)[j]     = (*pFailedIndexes)[j + 1];
            (*pFailedIndexes)[j + 1] = temp;
            swapped                  = true;
         }
      }
      if(!swapped) break;  // No swaps occurred, array is sorted
   }
}

static void CMPCsetupCompsComplete(int Index, COLvar Result, COLarray<int>* pFailedIndexes, SCKloop* pWorkLoop,
                                   std::shared_ptr<CMPCcreateTracker> pTracker) {
   COL_FUNCTION(CMPCsetupCompsComplete);
   COL_VAR(Result.json(1));
   pTracker->m_Count--;
   if(Result["success"].asBool()) {
      const COLstring Warning = Result["warning"].asString();
      if(!Warning.is_null()) { pTracker->m_Warning += Warning; }
      pTracker->m_Components[Index].CoreConfig.Commit = Result["commit"].asString();
      pTracker->m_Components[Index].CoreConfig.Repo   = Result["repo"].asString();
   } else {
      const COLstring Name  = pTracker->m_Components[Index].CoreConfig.Name;
      const COLstring Error = Result["error"].asString();
      pTracker->m_Error += "Could not setup " + Name + ". " + Error;
      pFailedIndexes->push_back(Index);
   }
   if(pTracker->m_Count == 0) {
      COL_TRC("Done setting up components");
      CMPCsortFailedIndexesAscending(pFailedIndexes);
      for(const auto& FailedIndex : *pFailedIndexes) {
         COL_TRC("Removing failed index: " << FailedIndex);
         pTracker->m_Components.remove(FailedIndex);
      }
      delete pFailedIndexes;
      pWorkLoop->shutdown();
   }
}

static void CMPCsetupComps(int Index, std::shared_ptr<CMPCcreateTracker> pTracker, SCKloop* pWorkLoop,
                           COLauto<COLclosure2<int, COLvar>> pCallback) {
   COL_FUNCTION(CMPCsetupComps);
   COLvar Result;
   Result["success"]          = true;
   CFGconfig       Config     = pTracker->m_Components[Index];
   CFGcore&        CoreConfig = Config.CoreConfig;
   const COLstring RepoDir    = DIRcomponentDevDir(CoreConfig.Guid);
   COLstring       Warning, Err;
   if(!CMPCsetupRepo(CoreConfig.Guid, pTracker->m_Plain, CoreConfig.Repo, CoreConfig.Branch, &CoreConfig.Commit, &Err)) {
      Result["success"] = false;
      Result["error"]   = Err;
      return pWorkLoop->post(COLnewClosure0(&CMPCcreateCallbackMain, Index, Result, pCallback));
   }
   CMPCcheckMainLuaFile(CoreConfig.Guid, &Warning);
   Result["warning"]        = Warning;
   Result["commit"]         = CoreConfig.Commit.is_null() ? "" : CoreConfig.Commit.substr(0, 7);
   Result["repo"]           = CoreConfig.Repo;
   const COLstring FileName = DIRcomponentConfig(CoreConfig.Guid);
   const COLstring DirPath  = FILpathDir(FileName);
   if(!FILfileExists(DirPath)) { FILmakeFullDir(DirPath, FIL_USER_RWX); }
   COL_TRC("Saving " << FileName);
   if(!FILsave(FileName, CFGconfigSerialize(Config).json(1), &Err)) {
      Result["success"] = false;
      Result["error"]   = Err;
   }
   pWorkLoop->post(COLnewClosure0(&CMPCcreateCallbackMain, Index, Result, pCallback));
}

static void CMPCsetupCacheComplete(int Index, COLvar Result, std::shared_ptr<CMPCcreateTracker> pTracker,
                                   SCKloop* pWorkLoop) {
   COL_FUNCTION(CMPCsetupCacheComplete);
   COL_VAR(Result.json(1));
   pTracker->m_Count--;
   if(!Result["success"].asBool()) {
      const COLstring Name  = pTracker->m_Components[Index].CoreConfig.Name;
      const COLstring Error = Result["error"].asString();
      pTracker->m_Error += " Could not setup cache for " + Name + ". " + Error;
   }
   if(pTracker->m_Count == 0) {
      COL_TRC("Done setting up cache dirs");
      if(pTracker->m_Error.is_null()) {
         COL_TRC("Begin component creation");
         pTracker->m_Count    = pTracker->m_Components.size();
         auto* pFailedIndexes = new COLarray<int>();
         for(int i = 0; i < pTracker->m_Components.size(); i++) {
            COLauto<COLclosure2<int, COLvar>> pCallback =
                COLnewClosure2(&CMPCsetupCompsComplete, pFailedIndexes, pWorkLoop, pTracker);
            pWorkLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPCsetupComps, i, pTracker, pWorkLoop, pCallback));
         }
      } else {
         COL_TRC("Failed early setting up cache dirs -- " << pTracker->m_Error);
         pTracker->m_FailedEarly = true;
         pWorkLoop->shutdown();
      }
   }
}

static void CMPCsetupCache(int Index, COLstring User, std::shared_ptr<CMPCcreateTracker> pTracker, SCKloop* pWorkLoop,
                           COLauto<COLclosure2<int, COLvar>> pCallback) {
   COL_FUNCTION(CMPCsetupCache);
   COLvar Result;
   Result["success"]        = true;
   const CFGconfig  Config  = pTracker->m_Components[Index];
   const COLstring& RepoDir = GITgitTemplateDir(Config.CoreConfig.Repo, Config.CoreConfig.Branch);
   if(FILfileExists(RepoDir)) {
      return pWorkLoop->post(COLnewClosure0(&CMPCcreateCallbackMain, Index, Result, pCallback));
   }
   COL_TRC("Setting up cache for " << Config.CoreConfig.Name << " at " << RepoDir);
   COLstring Output;
   if(!GITUcacheRepoFromUrl(User, Config.CoreConfig.Repo, Config.CoreConfig.Branch, &Output)) {
      Result["success"] = false;
      Result["error"]   = Output;
   }
   pWorkLoop->post(COLnewClosure0(&CMPCcreateCallbackMain, Index, Result, pCallback));
}

static void CMPCcreateConfiguration(CFGconfig* pConfig, const COLvar& Component, const COLstring& SourceId,
                                    const COLstring& ServerGuid, const bool PlainFolder) {
   COL_FUNCTION(CMPCcreateConfiguration);
   const COLstring StrippedName = COLstripChars(Component["name"], " ~!@#$%^&*()_+-=[]|\\;':,.<>/?\"");
   const COLstring Id           = StrippedName.substr(0, 10) + "_" + GUIDnewShortId();
   pConfig->CoreConfig          = CFGcore(Id, Component["name"], Component["description"], Component["tags"], true);
   pConfig->CoreConfig.Repo     = Component["git"];
   pConfig->CoreConfig.Branch   = Component["branch"];
   pConfig->CoreConfig.Commit   = "";  // set in CMPCsetupRepo
   pConfig->m_HostServerGuid    = ServerGuid;
   pConfig->Status.LastActivity = 0;
   bool Editable                = Component.exists("editable") && Component["editable"].asBool();
   if(pConfig->CoreConfig.Repo.is_null()) { Editable = true; }
   if(Editable) {
      pConfig->CoreConfig.Template = false;
   } else {
      pConfig->CoreConfig.Template = !PlainFolder;
   }
   if(!SourceId.is_null()) { pConfig->CoreConfig.Sources.push_back(SourceId); }
   COL_VAR(*pConfig);
}

static void CMPCcreateSetupArr(const COLvar& Components, COLarray<CFGconfig>* pCompArray, const bool Connected,
                               const COLstring& ServerGuid, const bool PlainFolder, const COLstring& Source) {
   COL_FUNCTION(CMPCcreateSetupArr);
   for(int i = 0; i < Components.size(); i++) {
      const COLstring& SourceId = Connected && i > 0 ? (*pCompArray)[i - 1].CoreConfig.Guid : Source;
      CMPCcreateConfiguration(&(*pCompArray)[i], Components[i], SourceId, ServerGuid, PlainFolder);
   }
}

static int CMPCgetNameSize(const COLstring& Name) {
   COL_FUNCTION(CMPCgetNameSize);
   int         Count   = 0;
   const char* pBuffer = Name.data();
   while(*pBuffer) {
      unsigned char c = static_cast<unsigned char>(*pBuffer);
      // Increment the count for a new character (non-continuation byte)
      if((c & 0x80) == 0 ||     // ASCII (1-byte character)
         (c & 0xC0) == 0xC0) {  // Start of a multibyte character
         Count++;
      }
      pBuffer++;
   }
   COL_VAR2(Count, Name.size());
   return Count;
}

// TODO component name size check should not include escape characters in the check see IX-1721
static bool CMPCcheckNames(const COLvar& Components, CFGmap* pMap, COLstring* pErr) {
   COL_FUNCTION(CMPCcheckNames);
   COLvar ComponentNames;
   CFGmapNames(pMap, &ComponentNames);
   COL_VAR(ComponentNames);
   COLmap<COLstring, bool> Lookup;
   *pErr = "Could not create components, ";
   for(int i = 0; i < Components.size(); i++) {
      const COLvar&   Component = Components[i];
      const COLstring Name      = COLvarRequiredString(Component, "name");
      COL_VAR2(Name, Component.exists(Name));
      if(Name.isWhitespace()) {
         *pErr += "component names must not be empty.";
         return false;
      }
      if(CMPCgetNameSize(Name) > 60) {
         *pErr += "component names must be less than 60 characters.";
         return false;
      }
      if(ComponentNames.exists(Name)) {
         *pErr += "name provided already exists in map: " + Name;
         return false;
      }
      if(Lookup.count(Name)) {
         *pErr += "cannot provide duplicate names for creation: " + Name;
         return false;
      }
      Lookup.add(Name, true);
      COLvarGetString(Component, "description", "");
      COLvarGetString(Component, "git", "");
   }
   return true;
}

static void CMPCcreateCallbackComplete(const COLarray<CFGconfig> Components, const COLvar Result, CFGmap* pMap,
                                       BLUAinstanceManager* pManager, SCKloop* pMainLoop, const COLaddress Address) {
   COL_FUNCTION(CMPCcreateCallbackComplete);
   COL_TRC("Component create complete");
   COL_VAR(Result.json(1));
   if(Result["failed_early"].asBool()) {
      return COLrespondError(Address, Result["error"].asString());
   }
   CMPCcreateFinish(Components, Result["warning"], Result["error"], pMap, pManager, pMainLoop, Address);
   if(!Result["plain"].asBool()) {
      COLclosure0* pFinal = COLnewClosure0(&CMPCcreateFinal);
      pMainLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPCcreateInstanceAdd, Components, pMainLoop,
         pFinal));
   }
}

static void CMPCcreateImpl(const COLvar Data, SCKloop* pMainLoop,
                           COLauto<COLclosure2<COLarray<CFGconfig>, COLvar>> pMainCallback) {
   const COLvar&    Components = Data["components"];
   const COLstring& User       = Data["user"];
   const bool&      Connected  = Data["connected"].asBool();
   const bool&      Plain      = Data["plain"].asBool();
   const COLstring& UniqueId   = Data["unique_id"];
   const COLstring& Source     = Data["source"];

   COLthreadPool Pool(1, 10);
   SCKloop       Loop(&Pool);
   Pool.start();
   auto pTracker = std::make_shared<CMPCcreateTracker>(User, Components.size(), Plain);
   CMPCcreateSetupArr(Components, &pTracker->m_Components, Connected, UniqueId, Plain, Source);
   for(int i = 0; i < pTracker->m_Components.size(); i++) {
      COLauto<COLclosure2<int, COLvar>> pCallback = COLnewClosure2(&CMPCsetupCacheComplete, pTracker, &Loop);
      Loop.threadPool()->scheduleTask(COLnewClosure0(&CMPCsetupCache, i, pTracker->m_User, pTracker, &Loop, pCallback));
   }
   Loop.start();
   COLvar Result;
   Result["warning"]      = pTracker->m_Warning;
   Result["error"]        = pTracker->m_Error;
   Result["plain"]        = pTracker->m_Plain;
   Result["failed_early"] = pTracker->m_FailedEarly;
   pMainLoop->post(COLnewClosure0(&CMPCcreateCallbackMain, pTracker->m_Components, Result, pMainCallback));
}

// /component/create
// TODO component name size check should not include escape characters in the check see IX-1721
void CMPCcreate(const COLwebRequest& Request, IWEBconfig* pWebConfig, CFGmap* pMap, BLUAinstanceManager* pManager,
                SCKloop* pLoop) {
   COL_FUNCTION(CMPCcreate);
   COL_VAR(Request.Data.json(1));

   const COLvar&   Components = COLvarRequiredObjectOrObjectList(Request.Data, "components");
   const bool      Connected  = COLvarGetBool(Request.Data, "connected", false);
   const bool      Plain      = COLvarGetBool(Request.Data, "plain", false);
   const COLstring Source     = COLvarGetString(Request.Data, "source", "");

   COLstring Err;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", CREATE_COMPONENT, &Err)) {
      return COLrespondError(Request.Address, PRMsetError(CREATE_COMPONENT));
   }
   if(!Source.is_null()) {
      CFGconfig* pConfig = CFGmapNode(pMap, Source);
      if(!pConfig) {
         return COLrespondError(Request.Address, "Source component does not exist: " + Source);
      }
      if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, ROLid::EDIT_CONNECTIONS, &Err)) {
         return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_CONNECTIONS));
      }
   }

   if(!DRXcomponentCountCheck(pMap->ConfigMap.size() + Components.size(), &Err)) {
      return COLrespondError(Request.Address, "Could not create components: " + Err);
   }
   if(!CMPCcheckNames(Components, pMap, &Err)) { return COLrespondError(Request.Address, Err); }
   COLvar Data;
   Data["user"]       = Request.User;
   Data["components"] = Components;
   Data["connected"]  = Connected;
   Data["plain"]      = Plain;
   Data["unique_id"]  = pWebConfig->uniqueId();
   Data["source"]     = Source;

   COLauto<COLclosure2<COLarray<CFGconfig>, COLvar>> pMainCallback =
       COLnewClosure2(&CMPCcreateCallbackComplete, pMap, pManager, pLoop, Request.Address);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&CMPCcreateImpl, Data, pLoop, pMainCallback));
}
