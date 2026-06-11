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

#include <EDI/EDIgrammar.h>  // switching to using full file names since it makes it easier to move code around
#include <EDI/EDIsave.h> 

#include <COL/COLsplit.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void EDIformatRequired(bool Required, COLstring* pOut){  
   COL_FUNCTION(EDIformatRequired);
   // By default, we assume a node is not required, and do not save req = false
   if (Required) { *pOut += COL_T(" ,\"req\":true"); }
}

// Composites do not support repeat property.
static void EDIformatRepeating(bool Repeating, EDItype Type, COLstring* pOut){
   COL_FUNCTION(EDIformatRepeating);
   // By default, we assume a node does not repeat, and do not save repeats = false
   if (Repeating && Type != EDI_COMPOSITE) { *pOut += COL_T(" ,\"repeats\":true"); }
}

static const char* EDIemptyBuffer = "                                                                                                                    ";

static void EDIpaddRow(COLstring* pRow, int Padding){
   COL_FUNCTION(EDIpaddRow);
   COLstring& Row = *pRow;
   if (Row.size() < Padding){
      Row.append(EDIemptyBuffer, Padding - Row.size());  // will break above a certain point.
   }
}

static int EDIfindMaxChildNameLength(const EDInode& Node){
   COL_FUNCTION(EDIfindMaxChildNameLength);
   int Max = 0;
   for (auto i=Node.Children.begin(); i != Node.Children.end(); ++i){
      Max = COL_MAX(Max, i->pNode->Name.size());
   } 
   return Max;
}

static int EDIfindMaxChildDesciptionLength(const EDInode& Node){
   COL_FUNCTION(EDIfindMaxChildDesciptionLength);
   int Max = 0;
   for (auto i=Node.Children.begin(); i != Node.Children.end(); ++i){
      Max = COL_MAX(Max, i->Description.size());
   } 
   return Max;
}

static void EDIoutputNode(const COLstring& Name, const EDInode& Node, COLstring* pOutput){
   COL_FUNCTION(EDIoutputNode);
   COLstring& Out = *pOutput;
   Out += COL_T(" ");
   COLappendJsonString(pOutput, Name, true, COLvar::StrEncJs);
   Out += COL_T(":{\n");
   Out += COL_T("  \"desc\":");
   COLappendJsonString(pOutput, Node.Description, true, COLvar::StrEncJs);
   Out += COL_T(",\n");
   Out += COL_T("  \"type\":\"");
   Out += EDItypeAsString(Node.Type);
   if (Node.Children.size() == 0){
      Out += COL_T("\"\n");
      Out += COL_T(" },\n");
      return;
   }
   Out += COL_T("\",\n");
   Out += "  \"children\":[\n";
   COLstring Row;
   int MaxNameLength = EDIfindMaxChildNameLength(Node);
   int MaxDescLength = EDIfindMaxChildDesciptionLength(Node);
   for (auto i=Node.Children.begin(); i != Node.Children.end(); ++i){
      Row = COL_T("   {\"type\":");
      COLappendJsonString(&Row, i->pNode->Name, true, COLvar::StrEncJs);
      Row += COL_T(",");
      EDIpaddRow(&Row, 15 + MaxNameLength);
      Row += COL_T("\"desc\":");
      COLappendJsonString(&Row, i->Description, true, COLvar::StrEncJs);
      EDIpaddRow(&Row, 24 + MaxNameLength + MaxDescLength);
      EDIformatRequired(i->Required, &Row);
      EDIformatRepeating(i->Repeats, Node.Type, &Row);
      Row += "},\n";
      Out += Row;   
   }
   Out.setSize(Out.size()-2);
   Out += "]},\n";
}

static void EDIoutputMatchingRule(const COLstring& Match, const COLstring& MessageName, const int MaxMatch, const int MaxMessageName, COLstring* pOutput){
   COL_FUNCTION(EDIoutputMatchingRule);
   COLstring& Out = *pOutput;
   COLstring Row;
   Row = COL_T(" ");
   Row += COL_T("{\"rule\":");
   COLappendJsonString(&Row, Match, true, COLvar::StrEncJs);
   EDIpaddRow(&Row, 11 + MaxMatch);
   Row += COL_T(", \"name\":");
   COLappendJsonString(&Row, MessageName, true, COLvar::StrEncJs);
   EDIpaddRow(&Row, 22 + MaxMatch + MaxMessageName);
   Out += Row + "},\n";
}

static void EDIsaveReportNodeError(const COLarray<COLstring>& BadNodes, COLstring* pError){
   COLstring Nodes;
   COLjoin(&Nodes, BadNodes, ",");
   *pError = "The vmd file contains the following nodes for which no definition was found: " + Nodes + ". Please add these nodes and try again.";
}

// https://interfaceware.atlassian.net/wiki/spaces/DEVELOPMEN/pages/2354249957/Configuration+format+and+Object+Model
bool EDIsave(const EDIcollection& Grammar, COLstring* pOutput, COLstring* pError){
   COL_FUNCTION(EDIsave);
   COLmap<COLstring, EDInode*> PreProcessed;
   for(auto i = Grammar.Nodes.cbegin(); i != Grammar.Nodes.cend(); ++i) {
      if(PreProcessed.count(i->first) == 0) { PreProcessed.add(i->first, i->second); }
      else if(PreProcessed[i->first]->Children.size() < i->second->Children.size()) {
         COL_TRC("Found duplicate with larger children -- " << i->first);
         PreProcessed[i->first] = i->second;
      }
   }
   COLarray<COLstring> BadNodes;
   *pOutput += "{\"nodes\":{\n";
   for (auto i=PreProcessed.cbegin(); i != PreProcessed.cend(); ++i){
      if (!(i->second)) { 
         BadNodes.push_back(i->first);
         continue;
      }
      EDIoutputNode(i->first, *i->second, pOutput);
   }
   if (BadNodes.size() > 0){
      EDIsaveReportNodeError(BadNodes, pError); // If bad nodes are found, abort saving. We don't want to convert the vmd by accident
      return false;
   }
   if (PreProcessed.size() > 0){
      pOutput->setSize(pOutput->size()-2); // strip of trailing ,
   }
   *pOutput += "},\n";
   *pOutput += "\"matching\":[\n";
   int MaxMatch = 0;
   int MaxMessageName = 0;
   for(const auto& MatchRule: Grammar.MatchRules) {
      MaxMatch       = COL_MAX(MaxMatch, MatchRule.Match.size());
      MaxMessageName = COL_MAX(MaxMessageName, MatchRule.MessageName.size());
   }
   for(const auto& MatchRule: Grammar.MatchRules) {
      EDIoutputMatchingRule(MatchRule.Match, MatchRule.MessageName, MaxMatch, MaxMessageName, pOutput);
   }
   if (Grammar.MatchRules.size() > 0){
      pOutput->setSize(pOutput->size()-2); // strip of trailing ,
   }
   *pOutput += "]\n}\n";
   return true;
}

COLostream& operator<<(COLostream& Stream, const EDIcollection& Col){
   COLstring Output, Error;
   bool Result = EDIsave(Col, &Output, &Error);
   Stream << Output;
   if (!Error.is_null()) Stream << "Error encountered: " << Error;
   return Stream;
}