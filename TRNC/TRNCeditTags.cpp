//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: TRNCeditTags.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   14/11/23 2:07 PM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfigSave.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <HASH/HASHutils.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKworker.h>
#include <TRNC/TRNCeditTags.h>
COL_LOG_MODULE;

struct TRNCeditTracker {
   int Count = 0;
   COLstring Errors;
   COLaddress Address;
};

static COLvar TRNCerrorVar(const COLstring& Error) {
   COLvar Var;
   Var["success"] = false;
   Var["error"]   = Error;
   return Var;
}

static void TRNCeditCleanup(TRNCeditTracker* pTracker) {
   COL_FUNCTION(TRNCeditCleanup);
   delete pTracker;
}

static void TRNCeditCallback(COLvar Result, COLstring Component, COLarray<COLstring> Tags, CFGmap* pMap,
                             TRNCeditTracker* pTracker) {
   COL_FUNCTION(TRNCeditCallback);
   COL_VAR(Result);
   pTracker->Count--;
   if(Result["success"].asBool()) {
      pMap->ConfigMap[Component].CoreConfig.Tags = Tags;
   } else {
      pTracker->Errors += Result["error"].asString() + "\n";
   }
   if(pTracker->Count == 0) {
      pTracker->Errors.is_null() ? COLrespondSuccess(pTracker->Address)
                                 : COLrespondError(pTracker->Address, pTracker->Errors);
      TRNCeditCleanup(pTracker);
   }
}

static void TRNCsaveWorker(const COLvar& In, COLvar* pOut) {
   COL_FUNCTION(TRNCsaveWorker);
   COLstring  Error;
   const bool Success = FILsave(DIRcomponentConfig(In["config"]["guid"]), In["config"].json(1), &Error);
   (*pOut)["success"] = Success;
   if(!Success) { (*pOut)["error"] = Error; }
}

static void TRNCearlyExit(const COLstring& Error, CFGmap* pMap, TRNCeditTracker* pTracker) {
   COL_FUNCTION(TRNCearlyExit);
   const COLarray<COLstring> Dummy;
   pTracker->Count = 1;
   TRNCeditCallback(TRNCerrorVar(Error), "", Dummy, pMap, pTracker);
}

void TRNCeditTagsUpdate(COLarray<COLstring>* pTags, const COLarray<COLstring>& AddVec,
                        const COLmap<COLstring, bool>& Lookup) {
   COL_FUNCTION(TRNCeditTagsUpdate);
   for(int i = pTags->size() - 1; i >= 0; i--) {
      COLstring Tag = (*pTags)[i];
      if(Lookup.count(Tag) == 1) {
         COL_TRC("Removing: " << Tag);
         pTags->remove(i);
      }
   }
   for(const auto& it : AddVec) {
      COL_TRC("Adding: " << it);
      pTags->push_back(it);
   }
}

static void TRNCeditTagsImpl(const COLvar& Components, const COLstring& ToAdd, const COLstring& ToRemove, CFGmap* pMap,
                             bool FromTranslator, const COLstring& User, SCKloop* pLoop, TRNCeditTracker* pTracker) {
   COL_FUNCTION(TRNCeditTagsImpl);
   COLarray<COLstring> AddVec, RemVec;
   COLsplit(&AddVec, ToAdd, " ");
   COLsplit(&RemVec, ToRemove, " ");
   COLmap<COLstring, bool> Lookup;
   for(const auto& it : AddVec) { if(!it.isWhitespace()) { Lookup[it] = true; } }
   for(const auto& it : RemVec) { if(!it.isWhitespace()) { Lookup[it] = true; } }
   for(const auto& it : Lookup) {
      if(!HASHisValid(it.first)) { return TRNCearlyExit(HASHerror(), pMap, pTracker); }
   }
   COLarray<COLstring> GoodComponents;
   COLarray<COLstring> BadComponents;
   CFGconvertNameToIdIfNeeded(Components, pMap, &GoodComponents, &BadComponents);
   for (const auto& BadComponent : BadComponents) {
      COLarray<COLstring> Dummy;
      TRNCeditCallback(TRNCerrorVar("No component exists with name or id " + BadComponent), BadComponent, Dummy, pMap, pTracker);
   }
   for(const auto& it : GoodComponents) {
      COLarray<COLstring> Dummy;
      const COLstring      Component = it;
      COLstring Error;
      if(!PRMcheckRoleAccess(FromTranslator, User, Component, ROLid::EDIT_DESCRIPTION, &Error)) {
         TRNCeditCallback(TRNCerrorVar(PRMsetError(ROLid::EDIT_DESCRIPTION)), Component, Dummy, pMap, pTracker);
         continue;
      }
      CFGconfig Config = pMap->ConfigMap[Component];
      TRNCeditTagsUpdate(&Config.CoreConfig.Tags, AddVec, Lookup);
      COLvar Data;
      Data["config"] = CFGconfigSerialize(Config);
      Data["user"]   = User;
      COLauto<COLclosure1<COLvar>> pCallback =
          COLnewClosure1(&TRNCeditCallback, Component, Config.CoreConfig.Tags, pMap, pTracker);
      COL_VAR(Data);
      SCKinvokeWorker(pLoop, TRNCsaveWorker, Data, pCallback);
   }
}

// component/edit/tags
void TRNCeditTags(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(TRNCeditTags);
   const COLvar Components    = COLvarRequiredObject(Request.Data, "components");
   const COLstring AddTags    = Request.Data["add"];
   const COLstring RemoveTags = Request.Data["remove"];
   if(!Components.isArray()) { return COLrespondError(Request.Address, "Invalid argument type for components"); }
   auto pTracker = new TRNCeditTracker();
   pTracker->Count = Components.size();
   pTracker->Address = Request.Address;
   TRNCeditTagsImpl(Components, AddTags, RemoveTags, pMap, Request.FromTranslator, Request.User, pLoop, pTracker);
}