//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Implementation
//
// Author:   Eliot Muir
// Date:     8th November 2022
//---------------------------------------------------------------------------

#include <DB/DBgrammarLoad.h>
#include <DB/DBvmdParser.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include <EDI/EDIload.h> // to check if vmd is binary
COL_LOG_MODULE;

DBsimpleType DBmapType(const COLstring& Type){
   COL_FUNCTION(DBmapType);
   if (Type == "string")   return DBS_STRING;
   if (Type == "integer")  return DBS_INTEGER;
   if (Type == "double")   return DBS_DOUBLE;
   if (Type == "datetime") return DBS_DATETIME;
   COL_ERR("Warning - unrecognized column types.");
   return DBS_STRING;
}

static bool DBloadColumns(DBtable* pTable, const COLvar& Columns, const COLstring& TableName, COLstring* pError) {
   COL_FUNCTION(DBloadColumns);
   if(!Columns.isArray()) { return true; }
   COLvar ProcessedColumns;
   COL_TRC("Creating columns nodes");
   for(int i = 0; i < Columns.size(); i++) {
      const COLvar& Column = Columns[i];
      if(!Column.isMap()) { continue; }
      COLstring Name    = Column["name"];
      if (!ProcessedColumns.exists(Name)) { ProcessedColumns[Name] = true; }
      else                                { *pError = "Encountered duplicate column: " + Name + " in table: " + TableName; return false; }
      COLstring Desc    = Column["desc"];
      bool IsKey        = Column.exists("key");
      DBsimpleType Type = DBmapType(Column["type"]);
      pTable->Columns.push_back(DBcolumn(Name, Desc, Type, IsKey));    
   }
   return true;
}

static bool DBloadTable(const COLstring& Name, const COLvar& NodeVar, DBcollection* pOutput, COLstring* pError) {
   COL_FUNCTION(DBloadTable);
   pOutput->Tables.add(Name, DBtable());
   pOutput->Tables[Name].Description = NodeVar["desc"];
   return DBloadColumns(&pOutput->Tables[Name], NodeVar["columns"], Name, pError);
}

bool DBload(const COLstring& JsonContent, DBcollection* pOutput, COLstring *pError){
   COL_FUNCTION(DBload);
   COL_DUMP("JSON:", JsonContent.c_str(), JsonContent.size());
   COLvar Json;
   COL_VAR(JsonContent);
   bool Success = Json.parse(JsonContent);
   if (!Success) { *pError = COL_T("invalid JSON"); }
   COL_VAR2(Json, Success);
   if(!Json.isMap()) {
      *pError = " format error - expected map but did not find one.";
      return false;
   }
   if (!Json.exists("tables")){
      *pError = " format error - could not find 'tables'.";
      COL_TRC("Empty set.");
      return true;  
   }
   const COLvar& Tables = Json["tables"];
   if(!Tables.isMap()) { COL_ERR("Tables should have been a map."); return false; }  
   pOutput->clear(); // just in case
   for(auto it = Tables.map().cbegin(); it != Tables.map().cend(); it ++) {
      COL_TRC("Attempting to load in " << it->first);
      if (!DBloadTable(it->first, it->second, pOutput, pError)){
         COL_TRC("Error processing table");
         return false;
      }
   }
   if (Json.exists("export")){
      const COLvar& Export = Json["export"];
      if (!Export.isMap()) { return false; }
      if (!Export.exists("datasource")) { return false; }
      pOutput->Export.add("datasource", Export["datasource"]);
   }
   return true;
}

// Can load a DBcollection from both new format (json) vdb files and legacy vmd files with tables defined
bool DBsmartLoad(const COLstring& Content, DBcollection* pOutput, COLstring* pError, bool* pLegacyWarning){
   COL_FUNCTION(DBsmartLoad);
   pOutput->clear(); // just in case 
   if(EDIisBinaryVmd(Content)) {
      *pError = "This vmd is in binary.  Please load it into Chameleon and save it as XML.";
      return false;
   }
   if(DBtranslateVmd(Content, pOutput)) {
      COL_TRC("We have a legacy vmd file.");
      *pLegacyWarning = true;
      return true;
   }
   COLstring ErrorMsg;
   bool Result = DBload(Content, pOutput, &ErrorMsg);
   *pLegacyWarning = false;
   if(!Result) { *pError = "Error loading vmd/vdb file: " + ErrorMsg; }
   return Result;
}
