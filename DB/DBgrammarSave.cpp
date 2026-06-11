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

#include <DB/DBgrammar.h>  // switching to using full file names since it makes it easier to move code around
#include <DB/DBgrammarSave.h> 

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void DBformatIsKey(bool IsKey, COLstring* pOut){  
   COL_FUNCTION(DBformatIsKey);
   // By default, we assume a node is not required, and do not save req = false
   if (IsKey) { *pOut += COL_T(" ,\"key\":1"); }
}

static void DBappendType(COLstring* pOut, DBsimpleType Type){
   COL_FUNCTION(DBappendType);
   switch(Type){
      case DBS_STRING:   *pOut += COL_T("\"string\""); break;
      case DBS_INTEGER:  *pOut += COL_T("\"integer\""); break;
      case DBS_DOUBLE:   *pOut += COL_T("\"double\""); break;
      case DBS_DATETIME: *pOut += COL_T("\"datetime\""); break;
   }
}

static const char* DBemptyBuffer = "                                                                                                                    ";

static void DBpaddRow(COLstring* pRow, int Padding){
   COL_FUNCTION(DBpaddRow);
   COLstring& Row = *pRow;
   if (Row.size() < Padding){
      Row.append(DBemptyBuffer, Padding - Row.size());  // will break above a certain point.
   }
}

static int DBfindMaxColumnNameLength(const DBtable& Table){
   COL_FUNCTION(DBfindMaxColumnNameLength);
   int Max = 0;
   for (auto i=Table.Columns.begin(); i != Table.Columns.end(); ++i){
      Max = COL_MAX(Max, i->Name.size());
   } 
   return Max;
}

static int DBfindMaxColumnDescriptionLength(const DBtable& Table){
   COL_FUNCTION(DBfindMaxColumnDescriptionLength);
   int Max = 0;
   for (auto i=Table.Columns.begin(); i != Table.Columns.end(); ++i){
      Max = COL_MAX(Max, i->Description.size());
   } 
   return Max;
}

static void DBoutputTable(const COLstring& Name, const DBtable& Table, COLstring* pOutput){
   COL_FUNCTION(DBoutputTable);
   COLstring& Out = *pOutput;
   Out += COL_T(" ");
   COLappendJsonString(pOutput, Name, true, COLvar::StrEncJs);
   if (Table.Columns.size() == 0){
      COL_TRC("No columns");
      Out += COL_T(":{\"desc\":");
      COLappendJsonString(pOutput, Table.Description, true, COLvar::StrEncJs);
      Out += COL_T("},\n");
      return;
   }
   Out += COL_T(":{\n");
   Out += COL_T("  \"desc\":");
   COLappendJsonString(pOutput, Table.Description, true, COLvar::StrEncJs);
   Out += COL_T(",\n  \"columns\":[\n");
   COLstring Row;
   int MaxNameLength = DBfindMaxColumnNameLength(Table);
   int MaxDescLength = DBfindMaxColumnDescriptionLength(Table);
   for (auto i=Table.Columns.begin(); i != Table.Columns.end(); ++i){
      Row = COL_T("   {\"type\":");
      DBappendType(&Row, i->Type);
      Row += COL_T(",");
      DBpaddRow(&Row, 22);
      Row += COL_T("\"name\":");
      COLappendJsonString(&Row, i->Name, true, COLvar::StrEncJs);
      Row += COL_T(",");
      DBpaddRow(&Row, 32 + MaxNameLength);
      Row += COL_T("\"desc\":");
      COLappendJsonString(&Row, i->Description, true, COLvar::StrEncJs);
      DBpaddRow(&Row, 41 + MaxNameLength + MaxDescLength);
      DBformatIsKey(i->IsKey, &Row);
      Row += "},\n";
      Out += Row;   
   }
   Out.setSize(Out.size()-2);
   Out += "]},\n";
}


static void DBsaveExport(const COLmap<COLstring, COLstring>& Export, COLstring* pOut){
   COL_FUNCTION(DBsaveExport);
   COLstring& Out = *pOut;
   Out += ",\n \"export\":{\n";
   Out += COL_T("   \"datasource\": \"") + Export["datasource"] + COL_T("\"\n}");
}

void DBsave(const DBcollection& Grammar, COLstring* pOutput){
   COL_FUNCTION(DBsave);
   if (Grammar.Tables.size() == 0){
      COL_TRC("Empty grammar");
      *pOutput += "{\"tables\":{}";
      if (Grammar.Export.size()){ DBsaveExport(Grammar.Export, pOutput); }
      *pOutput += "}\n";
      return;
   }
   *pOutput += "{\n \"tables\":{\n";
   for (auto i=Grammar.Tables.cbegin(); i != Grammar.Tables.cend(); ++i){
      DBoutputTable(i->first, i->second, pOutput);
   }
   pOutput->setSize(pOutput->size()-2);
   *pOutput += "\n}";
   if (Grammar.Export.size()){ DBsaveExport(Grammar.Export, pOutput); }
   *pOutput += "}";
   return;
}