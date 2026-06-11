//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LIBdata.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   10/06/25 12:32 PM
//  ---------------------------------------------------------------------------
#include <LIB/LIBdata.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
COL_LOG_MODULE;

static void LIBapplyUrls(const COLvar& DataVar, LIBurls* pUrls) {
   if(DataVar.exists("ssh") && !DataVar["ssh"].isNull()) { pUrls->m_Ssh = DataVar["ssh"]; }
   if(DataVar.exists("url") && !DataVar["url"].isNull()) { pUrls->m_Url = DataVar["url"]; }
   if(DataVar.exists("web") && !DataVar["web"].isNull()) { pUrls->m_Web = DataVar["web"]; }
}

static COLvar LIBurlsToVar(const LIBurls& Urls) {
   COLvar UrlVar;
   UrlVar["ssh"] = Urls.m_Ssh;
   UrlVar["web"] = Urls.m_Web;
   UrlVar["url"] = Urls.m_Url;
   COL_VAR(UrlVar.json(1));
   return UrlVar;
}

static COLvar LIBusageToVar(const LIBusage& Usage) {
   COL_FUNCTION(LIBusageToVar);
   COLvar UsageVar;
   UsageVar["component"]   = Usage.m_Component;
   UsageVar["path"]        = Usage.m_Path;
   UsageVar["error"]       = Usage.m_Error;
   UsageVar["ahead"]       = Usage.m_Ahead;
   UsageVar["behind"]      = Usage.m_Behind;
   UsageVar["has_changes"] = Usage.m_hasChanges;
   COL_VAR(UsageVar.json(1));
   return UsageVar;
}

void LIBdataToVar(const LIBdata& Data, COLvar* pOut) {
   COL_FUNCTION(LIBdataToVar);
   (*pOut)["name"]        = Data.m_Name;
   (*pOut)["description"] = Data.m_Description;
   (*pOut)["urls"]        = LIBurlsToVar(Data.m_Urls);
   (*pOut)["collection"]  = LIBurlsToVar(Data.m_CollectionUrls);
   (*pOut)["usage"].setArrayType();
   for(const auto& it : Data.m_Usage) { (*pOut)["usage"].push_back(LIBusageToVar(it)); }
   COL_VAR(pOut->json(1));
}

void LIBsetLibDataInfo(LIBdata* pData, const COLvar& DataVar) {
   COL_FUNCTION(LIBsetLibDataInfo);
   COL_VAR(DataVar.json(1));
   pData->m_Name        = DataVar["name"];
   pData->m_Description = DataVar["description"];
   LIBapplyUrls(DataVar["urls"], &pData->m_Urls);
   LIBapplyUrls(DataVar["collection"], &pData->m_CollectionUrls);
}

void LIBsetLibDataUsage(LIBdata* pData, const COLvar& DataVar) {
   COL_FUNCTION(LIBsetLibDataUsage);
   COL_VAR(DataVar.json(1));
   pData->m_Usage.push_back(LIBusage(DataVar["component"], DataVar["path"], DataVar["error"], DataVar["ahead"],
                                     DataVar["behind"], DataVar["has_changes"]));
   LIBapplyUrls(DataVar["urls"], &pData->m_Urls);
}
