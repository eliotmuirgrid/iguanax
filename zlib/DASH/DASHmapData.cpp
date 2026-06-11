//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DASHdashboardMapData.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   08/07/25 1:37 PM
//  ---------------------------------------------------------------------------
#include <CFG/CFGconfigSave.h>
#include <CFG/CFGmap.h>
#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <SDB/SDBcapi.h>
#include <DASH/DASHlistView.h>
#include <DASH/DASHmapData.h>
#include <DASH/DASHqueuePosition.h>
COL_LOG_MODULE;

static COLvar DASHcopyComponent(const CFGconfig& In) {
   COL_FUNCTION(DASHcopyComponent);
   COL_VAR(In.CoreConfig.Guid);
   COLvar Data;
   Data["guid"]            = In.CoreConfig.Guid;
   Data["name"]            = In.CoreConfig.Name;
   Data["template"]        = In.CoreConfig.Template;
   Data["border"]          = In.Status.Border;
   Data["tooltip"]         = In.Status.HtmlTooltipBlock;
   Data["light"]           = CFGoutputLight(In.Status.Light);
   Data["error_count"]     = In.Status.ErrorCount;
   Data["queue_count"]     = SDBIgetQueueCount(In.CoreConfig.Guid);
   Data["tags"]            = CFGconfigSerializeArray(In.CoreConfig.Tags);
   Data["sources"]         = CFGconfigSerializeArray(In.CoreConfig.Sources);
   Data["group"]           = false;
   Data["receivers"].setArrayType();
   return Data;
}

static void DASHcalculateReceivers(COLvar& Data) {
   COL_FUNCTION(DASHcalculateReceivers);
   if(!Data.isMap()) { return; }
   for (const auto& it : Data.map()) {
      for(int i = 0; i < it.second["sources"].size(); ++i) {
         COLstring SourceId = it.second["sources"][i];
         Data[SourceId]["receivers"].push_back(it.first);
      }
   }
}

static void DASHaddRelevantComponents(const COLhashmap<COLstring, COLstring>& OriginalList,
                                      const COLstring& ComponentId, const COLvar& ComponentList,
                                      COLhashmap<COLstring, COLstring>& RelevantIds) {
   COL_FUNCTION(DASHaddRelevantComponents);
   COL_VAR2(ComponentId, ComponentList.json(1));
   for(int i = 0; i < ComponentList.size(); i++) {
      if(OriginalList.count(ComponentList[i])) {
         COL_TRC("Adding : " << ComponentId);
         if(RelevantIds[ComponentId] == "node") continue;
         RelevantIds[ComponentId] = "neighbour";
      }
   }
}

static void DASHcopyAllData(COLvar& Alldata, CFGmap* pMap) {
   COL_FUNCTION(DASHcopyAllData);
   Alldata.setMapType();
   for(const auto& it : pMap->ConfigMap) { Alldata[it.first] = DASHcopyComponent(it.second); }
   COL_VAR(Alldata);
}

static void DASHcopyToMap(const COLarray<COLstring>& Components, COLhashmap<COLstring, COLstring>& OriginalMap) {
   COL_FUNCTION(DASHcopyToMap);
   for(const auto& Component : Components) { OriginalMap[Component] = "node"; }
}

static void DASHcopyRelevantData(const COLvar& AllData, const COLhashmap<COLstring, COLstring>& RelevantIds,
                                 COLvar& RelevantData) {
   COL_FUNCTION(DASHcopyRelevantData);
   for(const auto& it : RelevantIds) {
      COL_VAR2(it.first, it.second);
      if(it.second == "neighbour" || it.second == "node") {
         RelevantData[it.first]         = AllData[it.first];
         RelevantData[it.first]["type"] = it.second;
      }
   }
}

static void DASHaddComponentsNeighbours(const COLhashmap<COLstring, COLstring>& OriginalList, COLvar& AllData,
                                        COLhashmap<COLstring, COLstring>& RelevantIds) {
   COL_FUNCTION(DASHaddComponentsNeighbours);
   if(!AllData.isMap()) { return; }
   for(const auto& it : AllData.map()) {
      DASHaddRelevantComponents(OriginalList, it.first, it.second["sources"], RelevantIds);
      DASHaddRelevantComponents(OriginalList, it.first, it.second["receivers"], RelevantIds);
   }
}

static void DASHrequiresGrouping(const COLhashmap<COLstring, COLstring>& ExplicitlyShown, const COLvar& AllData,
                                 const COLvar& ComponentData, COLarray<COLstring>& ToReplace,
                                 COLarray<COLstring>& DontReplace) {
   COL_FUNCTION(DASHrequiresGrouping);
   COL_VAR(ComponentData);
   for(int i = 0; i < ComponentData.size(); i++) {
      COLstring ComponentId = ComponentData[i];
      COL_VAR2(AllData[ComponentId]["sources"].size(), AllData[ComponentId]["receivers"].size());
      const bool Replace = ExplicitlyShown.count(ComponentId) == 0 &&
                           AllData[ComponentId]["sources"].size() + AllData[ComponentId]["receivers"].size() <= 1;
      COL_VAR(Replace);
      Replace ? ToReplace.push_back(ComponentId) : DontReplace.push_back(ComponentId);
   }
}

void DASHgroupComponents(const COLhashmap<COLstring, COLstring>& OriginalList,
                         const COLhashmap<COLstring, COLstring>& ExplicitlyShown, const COLstring& Group,
                         COLhashmap<COLstring, COLstring>& RelevantIds, COLvar& AllData, COLvar& GroupData) {
   COL_FUNCTION(DASHgroupComponents);
   const COLstring GroupingTo = Group == "receivers" ? "sources" : "receivers";
   for(const auto& it : OriginalList) {
      COLarray<COLstring> ToReplace, DontReplace;
      DASHrequiresGrouping(ExplicitlyShown, AllData, AllData[it.first][Group], ToReplace, DontReplace);
      if(ToReplace.size() < 8) { continue; }
      COL_TRC("Grouping the sources of component: " << it.first);
      COLvar NewGroupData;
      COLvarConvertFromArray(ToReplace, &NewGroupData["replace"]);
      NewGroupData[Group].setArrayType();
      NewGroupData[GroupingTo].setArrayType().push_back(it.first);
      NewGroupData["group"]   = true;
      const COLstring GroupId = "grp-" + Group + "-" + it.first;
      NewGroupData["guid"]    = GroupId;
      GroupData[GroupId]      = NewGroupData;
      DontReplace.push_back(GroupId);
      COLvarConvertFromArray(DontReplace, &AllData[it.first][Group]);
      for(int i = 0; i < NewGroupData["replace"].size(); i++) {
         COLstring ReplacedId = NewGroupData["replace"][i];
         COL_VAR(ReplacedId);
         RelevantIds[ReplacedId] = "group";
      }
   }
}

static COLarray<COLstring> DASHmapGetSelected(const COLvar& Selected, const bool Group, CFGmap* pMap) {
   COL_FUNCTION(DASHmapGetSelected);
   if(Group) { return DASHvarToArray(Selected); }
   COLarray<COLstring> SelectedArr;
   for(const auto& it : pMap->ConfigMap) { SelectedArr.push_back(it.first); }
   return SelectedArr;
}

void DASHmapDataImp(const COLstring& GlobTerms, const bool ExactMatch, const COLarray<COLstring>& Selected,
                    const bool OnlyChecked, CFGmap* pMap, COLvar& Result) {
   COL_FUNCTION(DASHmapDataImp);
   COLarray<COLstring> Components;
   COLvar AllData;
   DASHcopyAllData(AllData, pMap);
   DASHcalculateReceivers(AllData);
   COLhashmap<COLstring,COLstring> ExplicitlyShown;
   DASHcopyToMap(Selected, ExplicitlyShown);
   if (!OnlyChecked) { DASHglobMatch(GlobTerms, pMap, ExactMatch, &Components); }
   DASHcombineLists(Selected, pMap, &Components);
   COLhashmap<COLstring,COLstring> OriginalList;
   DASHcopyToMap(Components, OriginalList);
   COLhashmap<COLstring,COLstring> RelevantIds = OriginalList;
   DASHaddComponentsNeighbours(OriginalList, AllData, RelevantIds);
   // DASHgroupComponents(OriginalList, ExplicitlyShown, "sources", RelevantIds, AllData, Result);
   // DASHgroupComponents(OriginalList, ExplicitlyShown, "receivers", RelevantIds, AllData, Result);
   DASHcopyRelevantData(AllData, RelevantIds, Result);
}

// /component/map
void DASHmapData(const COLwebRequest& Request, CFGmap* pMap) {
   COL_FUNCTION(DASHmapData);
   COL_VAR(Request.Data.json(1));
   COLvar Result;
   const COLstring GlobTerms   = COLvarGetString(Request.Data, "filter", "");
   const COLvar    Selected    = COLvarGetObject(Request.Data, "checked", COLvar());
   const bool      ExactMatch  = COLvarGetBool(Request.Data, "whole_word", false);
   const bool      Group       = COLvarGetBool(Request.Data, "group", true);
   const bool      OnlyChecked = COLvarGetBool(Request.Data, "checked_only", false);
   DASHmapDataImp(GlobTerms, ExactMatch, DASHmapGetSelected(Selected, Group, pMap), OnlyChecked, pMap, Result);
   COLrespondSuccess(Request.Address, Result);
}
