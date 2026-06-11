//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Implementation
//
// Author:   Eliot Muir
// Date:     26th July 2022
//---------------------------------------------------------------------------

#include <EDI/EDIgrammar.h>
#include <EDI/EDIload.h> 
#include <EDI/EDIvmdParser.h>

#include <FIL/FILutils.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void EDIcreateChildren(EDInode* pNode, const COLvar& Children, EDIcollection* pOutput) {
   COL_FUNCTION(EDIcreateChildren);
   pNode->Children.clear();
   if(!Children.isArray()) { return; }
   COL_TRC("Creating child nodes");
   for(int i = 0; i < Children.size(); i++) {
      if(!Children[i].isMap()) { continue; }
      if(Children[i].exists("type") && pOutput->Nodes.count(Children[i]["type"])) {
         EDIchild Child(pOutput->Nodes[Children[i]["type"]], Children[i]["desc"]);
         if(Children[i].exists("repeats")) { Child.Repeats  = Children[i]["repeats"]; }
         if(Children[i].exists("req"))     { Child.Required = Children[i]["req"]; }
         pNode->Children.push_back(Child);
      }
   }
}

static void EDIcreateNode(const COLstring& Name, const COLvar& NodeVar, EDIcollection* pOutput) {
   COL_FUNCTION(EDIcreateNode);
   auto pNode = new EDInode();
   pNode->Name = Name;
   pNode->Description = NodeVar["desc"];
   pNode->Type        = EDIstringToType(NodeVar["type"]);
   pOutput->Nodes.add(Name, pNode);
}

static void EDIcreateRule(const COLstring& Match, const COLstring& MessageName, EDIcollection* pOutput){
   COL_FUNCTION(EDIcreateRule);
   EDImatch Rule;
   Rule.Match = Match;
   Rule.MessageName = MessageName;
   COL_VAR2(Match, MessageName);
   pOutput->MatchRules.push_back(Rule);
}

bool EDIload(const COLstring& JsonContent, EDIcollection* pOutput, COLstring* pError){
   COL_FUNCTION(EDIload);
   COLvar Json;
   COL_VAR(JsonContent);
   if (!Json.parse(JsonContent)){
      *pError = COL_T("invalid JSON");
      return false;
   }
   COL_VAR(Json);
   if(!Json.isMap()) {
      *pError = " format error - expected map but did not find one.";
      return false;
   }
   if(!Json.exists("nodes") || !Json.exists("matching")) {
      *pError = " format error - could not find 'nodes' or 'matching'.";
      return false;
   }
   for(auto it = Json["nodes"].map().cbegin(); it != Json["nodes"].map().cend(); it ++) {
      COL_TRC("Attempting to load in " << it->first);
      EDIcreateNode(it->first, it->second, pOutput);
   }
   for(auto it = Json["nodes"].map().cbegin(); it != Json["nodes"].map().cend(); it ++) {
      EDIcreateChildren(pOutput->Nodes[it->first], it->second["children"], pOutput);
   }
   for(int i = 0; i < Json["matching"].size(); i++){
      EDIcreateRule(Json["matching"][i]["rule"], Json["matching"][i]["name"], pOutput);
   }
   return true;
}

bool EDIisBinaryVmd(const COLstring& Content){
   COL_FUNCTION(EDIisBinaryVmd);
   if (Content.size() < 3){
      return false;
   }
   if (Content[0] == 0x60 && Content[1] == 0x34){
      COL_TRC("We have a binary vmd");
      return true;
   }
   return false;
}

// This can handle vmd or new JSON format
bool EDIsmartLoad(const COLstring& Content, EDIcollection* pOutput, COLstring* pError, COLstring* pWarning, bool* pLegacyWarning){
   COL_FUNCTION(EDIsmartLoad);
   pOutput->clear(); // just in case
   *pLegacyWarning = false;
   if(EDIisBinaryVmd(Content)) {
      *pError = "This vmd is in binary.  Please load it into Chameleon and save it as XML.";
      return false;
   }
   // TODO could use a safer more lightweight test
   if(EDItranslateVmd(Content, pOutput, pWarning)) {
      COL_VAR(*pWarning);
      *pLegacyWarning = true;
      return true;
   }
   COLstring ErrorMsg;
   bool LoadSuccess = EDIload(Content, pOutput, &ErrorMsg);
   if (!LoadSuccess) { *pError = "Error loading vmd file: " + ErrorMsg; }
   return LoadSuccess;
}

// We have to strip out the Javascript stuff as the beginning and end to get the pure JSON.
bool EDIloadWeb(const COLstring& FileName, EDIcollection* pCollection, COLstring* pError){
   COL_FUNCTION(EDIloadWeb);
   COLstring Content;
   FILreadFile(FileName, &Content);
   COL_DUMP("Json Data:", Content.c_str(), 200);
   int FirstNL =0;
   if (Content.find(&FirstNL, "{")){
      COL_VAR(FirstNL);
   }
   int LastNL = Content.size()-1;
   if (Content.rfind(&LastNL, "}")){
      COL_VAR(LastNL);
   }
   COLstring Json = Content.substr(FirstNL, LastNL-FirstNL+1);
   COL_DUMP("Json:", Json.c_str(), 30);
   COL_DUMP("End Json:", Json.c_str()+ Json.size()-20, 20);
   return EDIload(Json, pCollection, pError);
}