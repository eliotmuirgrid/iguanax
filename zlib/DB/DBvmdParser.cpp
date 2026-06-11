//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Implementation
//
// Author:   Vismay Shah
// Date:     13th March 2023
//---------------------------------------------------------------------------
#include <DB/DBvmdParser.h> 
#include <DB/DBgrammar.h>
#include <DB/DBgrammarLoad.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool DBparseName(const COLvar& Table, COLstring* pName){
   COL_FUNCTION(DBparseName);
   if (Table["@"].exists("name")) {
      *pName = Table["@"]["name"].asString();
      return true;
   } 
   return false;
}

static void DBparseDescription(const COLvar& Table, COLstring* pTableDescription){
   COL_FUNCTION(DBparseDescription);
   if (Table.exists("description") && Table["description"].isArray() && Table["description"].size() > 0 && Table["description"][0].exists("text")){
      *pTableDescription = Table["description"][0]["text"].asString();
   } 
}

static bool DBparseColumnType(const COLvar& Table, COLstring* pType){
   COL_FUNCTION(DBparseColumnType);
   if (Table[0].exists("text")) {
      *pType = Table[0]["text"].asString().toLowerCase();
      return true;
   } 
   return false;
}

static bool DBparseColumns(const COLvar& SourceColumns, DBtable& ParentTable){
   COL_FUNCTION(DBparseColumns);
   for (int i = 0; i < SourceColumns.size(); i++){
      const COLvar& CurrentColumn = SourceColumns[i];
      bool ColumnParseSuccess = true;
      COLstring Name, Description, Type;
      if (CurrentColumn.exists("@") && CurrentColumn["@"].isMap()) { ColumnParseSuccess &= DBparseName(CurrentColumn, &Name); } 
      DBparseDescription(CurrentColumn, &Description);
      if (CurrentColumn.exists("type") && CurrentColumn["type"].isArray()) { ColumnParseSuccess &= DBparseColumnType(CurrentColumn["type"], &Type); }
      if (!ColumnParseSuccess) {
         COL_TRC("Failed to parse column");
         return false;
      } else {
         ParentTable.Columns.push_back(DBcolumn(Name, Description, DBmapType(Type), false)); // Unfortunately, the is_key parameter is elsewhere in the vmd. Default to false and update it later in the parsing.
      }
   }
   return true;
}

static void DBparseTable(const COLvar& Table, DBcollection* pOutput){
   COL_FUNCTION(DBparseTable);
   if (!Table.isMap()) { COL_TRC("Format error - table not map."); return; }
   COLstring Name, Description;
   if (Table.exists("@") && Table["@"].isMap() && Table.exists("column") && Table["column"].isArray()) {
      if(!DBparseName(Table, &Name)) { COL_TRC("Format error - table with no name."); return; } // table must have a name
      COL_TRC("Found table " << Name);
      DBparseDescription(Table, &Description);
      DBtable CurrentTable;
      CurrentTable.Description = Description;
      if (DBparseColumns(Table["column"], CurrentTable)){
         COL_TRC("Successfully parsed table " << Name);
         pOutput->Tables.add(Name, CurrentTable);
      }
   }
}

static bool DBgetTablesFromGlobals(const COLvar& Globals, DBcollection* pOutput){
   COL_FUNCTION(DBgetTablesFromGlobals);
   if (!Globals.isArray()) { return false; }
   for(int i = 0; i < Globals.size(); i++){
      if (Globals[i].exists("table")){
         COL_TRC("Trying to parse table...");
         for (int j = 0; j < Globals[i]["table"].size(); j++){
            DBparseTable(Globals[i]["table"][j], pOutput);
         }
      }
   }
   return true;
}

static void DBgetIsKey(const COLvar& Column, bool* pIsKey){
   COL_FUNCTION(DBgetIsKey);
   if (Column.exists("config") && Column["config"].isArray() && Column["config"][0].exists("is_key")
       && Column["config"][0]["is_key"][0].exists("text")){

      if (Column["config"][0]["is_key"][0]["text"].asString() == "True")       { *pIsKey = true;  }
      else if (Column["config"][0]["is_key"][0]["text"].asString() == "False") { *pIsKey = false; }
   }
}

static void DBparseKeyColumn(const COLvar& Columns, DBtable& Output){
   COL_FUNCTION(DBparseKeyColumn);
   for (int i = 0; i < Columns.size(); i++){
      COLstring ColumnName;
      DBparseName(Columns[i], &ColumnName);
      COL_VAR(ColumnName);
      if (ColumnName.is_null()) { continue; }
      bool IsKey = false;
      DBgetIsKey(Columns[i], &IsKey);
      for (int j = 0; j < Output.Columns.size(); j++) {
         if (Output.Columns[j].Name == ColumnName) { 
            COL_TRC("Updating column " << ColumnName);
            Output.Columns[j].IsKey = IsKey; 
         }
      }
   }
}

static bool DBgetKeyColumns(const COLvar& Config, DBcollection* pOutput){
   COL_FUNCTION(DBgetKeyColumns);
   if ( !(Config.exists("table") && Config["table"].isArray()) ) { return false; }
   for (int i = 0; i < Config["table"].size(); i++) {
      COLstring CurrentTableName;
      if (Config["table"][i].exists("@") && Config["table"][i]["@"].isMap()){ DBparseName(Config["table"][i], &CurrentTableName); } 
      else                                                                  { COL_TRC("Unexpected format encountered, skipping."); continue; }
      if (pOutput->Tables.count(CurrentTableName) && Config["table"][i].exists("column") && Config["table"][i]["column"].isArray()) {
         DBparseKeyColumn(Config["table"][i]["column"], pOutput->Tables[CurrentTableName]);
      }
   }
   return true;
}

bool DBtranslateVmd(const COLstring& Content, DBcollection* pOutput){
   COL_FUNCTION(DBtranslateVmd);
   COLvar ParsedXml;
   if (!ParsedXml.parseXml(Content)) { return false; }
   if (!DBgetTablesFromGlobals(ParsedXml["engine"]["global"], pOutput)) { return false; }
   COL_TRC("Found " << pOutput->Tables.size() << " tables.");
   if (pOutput->Tables.size() == 0) { // small optimization - if we have no tables don't bother looking for is_key
      COL_TRC("Didn't find any tables");
      return true;
   }
   COL_TRC("Now trying to get is_key for all the columns");
   int CurrentConfig = ParsedXml["engine"]["current_config"][0]["text"];
   COL_TRC("CURRENT CONFIG: " << CurrentConfig);
   const COLvar& Config = ParsedXml["engine"]["config"][CurrentConfig];
   if(!Config.isMap()) { return false; }
   if (!DBgetKeyColumns(Config, pOutput)) { return false; }
   return true;
}
