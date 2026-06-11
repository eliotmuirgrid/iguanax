// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEazureUtils
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Thursday 16 May 2024 - 12:53PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLvar.h>
#include <GITE/GITEazureUtils.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

bool GITEazureExtraLogging(){
   COL_FUNCTION(GITEazureExtraLogging);
   bool ExtraLogging = false;
   if (getenv("IFW_AZURE_EXTRA_LOGGING")){ ExtraLogging = true; }
   return ExtraLogging;
}

bool GITEazureEntitlementsParseUser(const COLvar& UserData, COLstring* pResult){
   COL_FUNCTION(GITEazureEntitlementsParseUser);
   bool IdExists = UserData.isMap() && UserData.exists("items") && UserData["items"].isArray() && UserData["items"].size() > 0 &&
                   UserData["items"][0].isMap() && UserData["items"][0].exists("id");
   COL_VAR(IdExists);
   if (IdExists){
      COL_TRC("Found id");
      *pResult = UserData["items"][0]["id"];
   }
   return IdExists;
}

bool GITEazureParseOrganizations(const WEBresponse& Data, COLarray<COLstring>* pResult, COLstring* pError){
   COL_FUNCTION(GITEazureParseOrganizations);
   COL_VAR(Data);
   bool ExtraLogging = GITEazureExtraLogging();
   if (ExtraLogging) { SDBIlogService("ParseOrganizations - Code:" + COLintToString(Data.ReturnCode) + "\n Raw response: " + Data.Body); }
   COLvar Out;
   if (!Out.parse(Data.Body)) {
      COL_VAR(Data.Body);
      COL_TRC("Failed to parse organizaion json");
      if(ExtraLogging) { SDBIlogService("ParseOrganizations encountered error - Failed to parse json."); }
      *pError = "Could not parse organization data";
      return false;
   }
   if (!Out.isMap() || !Out.exists("value") || !Out["value"].isArray()){
      COL_TRC("Expected array but got something else");
      if(ExtraLogging) { SDBIlogService("ParseOrganizations encountered error - Did not get correct format."); }
      *pError = "Could not parse organization data";
      return false;
   }
   Out = Out["value"];
   for (int i = 0; i < Out.array().size(); i++){
      if (Out[i].isMap()){
			COLstring Name;
			// check below because local api responds with collections, where key is name
			if(Out[i].exists("accountName")) { Name = Out[i]["accountName"]; }
			else if(Out[i].exists("name"))   { Name = Out[i]["name"]; }
         COL_TRC("Found organization: " + Name);
         pResult->push_back(Name);
      }
   }
   if(ExtraLogging) {
      SDBIlogService("Response format ok. Total organizations: " + COLintToString(pResult->size()));
      COLstring Orgs;
      COLjoin(&Orgs, *pResult, "\n");
      SDBIlogService("Organizations: " + Orgs);
   }
   return true;
}

bool GITEazureParseProjects(const WEBresponse& Data, const COLstring& OrganizationName, COLvar* pResult, COLstring* pError){
   COL_FUNCTION(GITEazureParseProjects);
   bool ExtraLogging = GITEazureExtraLogging();
   if (ExtraLogging) { SDBIlogService("ParseProjects - Code:" + COLintToString(Data.ReturnCode) + "\n Raw response: " + Data.Body); }
   COLvar Out;
   if (!Out.parse(Data.Body)) {
      COL_TRC("Failed to parse projects json");
      if(ExtraLogging) { SDBIlogService("ParseProjects encountered error - Failed to parse json."); }
      *pError = "Could not parse projects data";
      return false;
   }
   if (!Out.isMap() || !Out.exists("value") || !Out["value"].isArray()){
      COL_TRC("Expected array but got something else");
      if(ExtraLogging) { SDBIlogService("ParseProjects encountered error - Did not get correct format."); }
      *pError = "Could not parse projects data";
      return false;
   }
   Out = Out["value"];
   for (int i = 0; i < Out.array().size(); i++){
      if (Out[i].isMap() && Out[i].exists("name")){
         COL_TRC("Found project: " + Out[i]["name"].asString());
         pResult->push_back(OrganizationName + COL_T("/") + Out[i]["name"].asString());
      }
   }
   if(ExtraLogging) {
      SDBIlogService("Response format ok. Total owners: " + COLintToString(pResult->size()));
      COLstring Owners;
      COLjoin(&Owners, *pResult, "\n");
      SDBIlogService("Owners: " + Owners);
   }
   return true;
}