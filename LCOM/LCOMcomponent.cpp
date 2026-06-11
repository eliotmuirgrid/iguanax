//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMcomponent
//
// component lua name space
//
// Author: Shuo Hou
// Date:   Feb 18th, 2022
//---------------------------------------------------------------------------
#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAqueue.h>
#include <COL/COLlog.h>
#include <LCOM/LCOMclone.h>
#include <LCOM/LCOMcomponent.h>
#include <LCOM/LCOMgetDescription.h>
#include <LCOM/LCOMgetDirectory.h>
#include <LCOM/LCOMgetFields.h>
#include <LCOM/LCOMgetId.h>
#include <LCOM/LCOMgetProjectDir.h>
#include <LCOM/LCOMgetTags.h>
#include <LCOM/LCOMisLive.h>
#include <LCOM/LCOMgetName.h>
#include <LCOM/LCOMrunAs.h>
#include <LCOM/LCOMsetField.h>
#include <LCOM/LCOMsetStopOnError.h>
#include <LCOM/LCOMstopping.h>
#include <LCOM/LCOMstopComponent.h>
#include <LIG/LIGcallComponentFunction.h>
#include <LUA/LUAtimeout.h>
#include <LUI/LUIsetBorderColor.h>
#include <LUI/LUIsetDashboardColumn.h>
#include <LUI/LUIsetStatusMessage.h>
#include <LUI/LUIsetTooltip.h>
#include <LTMR/LTMRtimer.h>
#include <LUA/LUAheader.h>
#include <LUA/LUAutils.h>
COL_LOG_MODULE;

void LCOMaddComponentNamespace(lua_State* L, CFGmap* pMap, BLUAinstanceManager* pBluaManager, SCKloop* pLoop){
   COL_FUNCTION(LCOMaddComponentNamespace);
   lua_newtable(L);
   LUAaddMethod(L, "fields",          &LCOMgetFields);
   LUAaddMethod(L, "setField",        &LCOMsetField);
   LUAaddMethod(L, "setBorder",       &LUIsetBorderColor);
   LUAaddMethod(L, "setStatus",       &LUIsetStatusMessage);
   LUAaddMethod(L, "setTooltip",      &LUIsetTooltip);
   LUAaddMethod(L, "live",            &LCOMisProductionMode);
   LUAaddMethod(L, "setTimer",        &LTMRsetTimer);
   LUAaddMethod(L, "cancelTimer",     &LTMRcancelTimer);
   LUAaddMethod(L, "retry",           &BLUAretryDequeue);
   LUAaddMethod(L, "retryMessage",    &BLUAretryDequeue);
   LUAaddMethod(L, "setLight",        &BLUAsetLight);
   LUAaddMethod(L, "isStopping",      &LCOMisStopping);
   LUAaddMethod(L, "name",            &LCOMgetName);
   LUAaddMethod(L, "description",     &LCOMgetDescription);
   LUAaddMethod(L, "id",              &LCOMgetId);
   LUAaddMethod(L, "tags",            &LCOMgetTags);
   LUAaddMethod(L, "runAs",           &LCOMrunAsUser);
   LUAaddMethod(L, "runAsUser",       &LCOMrunAsUser);
   // Important! setup call before registering its method. Same applies to clone
   LIGsetUpComponentCall(L, pMap, pBluaManager, pLoop);
   LUAaddMethod(L, "call",            &LIGcallComponentFunction);
   LCOMsetUpClone(L, pBluaManager, pLoop);
   LUAaddMethod(L, "clone",            &LCOMspawnClone);
   LUAaddMethod(L, "spawnClone",       &LCOMspawnClone);
   LUAaddMethod(L, "setColumn",        &LUIsetDashboardColumn);
   LUAaddMethod(L, "dir",              &LCOMgetDirectory);
   LUAaddMethod(L, "setStopOnError",   &LCOMsetStopOnError);
   LUAaddMethod(L, "setScriptTimeout", &LUAsetTimeout);
   LUAaddMethod(L, "projectRoot",      &LCOMgetProjectDir);
   LUAaddMethod(L, "stop",             &LCOMstopComponent);
   lua_setglobal(L, "component");
}
