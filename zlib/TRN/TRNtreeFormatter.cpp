//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtreeFormatter
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Friday, June 25th, 2010 @ 09:27:47 AM
//
//---------------------------------------------------------------------------
#include "TRNhl7Formatter.h"
#include "TRNtreeFormatter.h"
#include <THTM/THTMprinter.h>
#include "TRNconstant.h"
#include "TRNtraverser.h"

#include <NOD/NODplace.h>
#include <NOD/NODprintCallback.h>

#include <LUAL/LUALutils.h>

#include <NOB/NOBmemoizer.h>
#include <NOB/NOBcomplexLuaNode.h>

#include <NOX/NOXconstant.h>
#include <NOX/NOXtreeValue.h>
#include <NODB/NODBconstant.h>
#include <NODB/NODBtreeValue.h>
#include <NTAB/NTABconstant.h>
#include <NTAB/NTABtreeValue.h>
#include <NTV/NTVtreeValue.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


// Print " (X Unit[s])" to the specified stream.
void TRNprintCountAndUnits(COLostream& Stream, int Count, const char* pUnit){
   Stream << COL_T(" (") << Count << ' ' << pUnit;
   if (Count != 1) Stream << 's';
   Stream << ')';
}

TRNtreeFormatter::TRNtreeFormatter()
: m_TopIndex(-1)
{
}

void TRNtreeFormatter::pushNode(const NODplace& Node, int GrammarIndex){
   m_Stack.push_back(&Node);
   m_TopIndex = GrammarIndex;
}

void TRNtreeFormatter::popNode(){
   m_Stack.pop_back();
}

const NODplace& TRNtreeFormatter::topNode() const{
   COLPRECONDITION(m_Stack.size());
   COLPRECONDITION(m_Stack[m_Stack.size()-1]);
   return *m_Stack[m_Stack.size()-1];
}

const NODplace* TRNtreeFormatter::secondFromTopNode() const{
   if (m_Stack.size() > 1) {
      return m_Stack[m_Stack.size()-2];
   }else{
      return NULL;
   }
}

static void TRNprintLuaNode(const NODplace& LuaNode, bool Expanded, COLostream& Stream){
   if (const NOBcomplexLuaNode* pComplexLuaNode = dynamic_cast<const NOBcomplexLuaNode*>(&LuaNode)){
      Stream << '[' << LuaNode.name() << ']';
      if (Expanded){
         if (!pComplexLuaNode->shortLabel().is_null()) Stream << '=' << pComplexLuaNode->shortLabel();
      } else if (!pComplexLuaNode->longLabel().is_null()){
         if (!pComplexLuaNode->longLabel().is_null()) Stream << '=' << pComplexLuaNode->longLabel();
      }
   } else if (LuaNode.protocolType() != LUAL_LUA_PROTOCOL){
      // We have an actual NODplace (not just a branch of the reconstructed Lua tree).
      const NOBmemoizer* pMemoizer = NOBgetMemoizer(LuaNode);
      COLPRECONDITION(pMemoizer);
      Stream << '[' << pMemoizer->keyName() << COL_T("]=");
      // We create a temporary traverser for this node, in order to get the treeValue for the
      // collapsed view.  This isn't the most efficient code, but it's not something we do often.
      COLauto<TRNtraverser> pTempTraverser = TRNcreateTraverser(LuaNode);
      COLstring NodeCollapsedView, NodeExpandedView;
      pTempTraverser->getTreeviewText(NodeExpandedView, NodeCollapsedView);
      if (Expanded){
         Stream << NodeExpandedView;
      } else {
         Stream << NodeCollapsedView;
      }
   } else {
      Stream << '[' << LuaNode.name() << ']';
      if (!Expanded){
         Stream << '=';
         if (LuaNode.isSimple()){
            Stream << LuaNode.value();
         } else {
            Stream << '{';
            for (int i = 0, n = LuaNode.size(); i < n; i++){
               if (i) Stream << COL_T(", ");
               TRNprintLuaNode(LuaNode.child(i), false, Stream);
            }
            Stream << '}';
         }
      }
   }
}

COLstring TRNluaFormatter::view(bool Expanded) const{
   if (m_Stack.size() == 1){
      const NODplace& Top = topNode();
      if (const NOBcomplexLuaNode* pComplexLuaNode = dynamic_cast<const NOBcomplexLuaNode*>(&Top)){
         if (Expanded){
            return pComplexLuaNode->shortLabel();
         } else {
            return pComplexLuaNode->longLabel().is_null() ? pComplexLuaNode->shortLabel() : pComplexLuaNode->longLabel();
         }
      } else {
         return Top.nodeTypeName();
      }
   } else {
      const NODplace& Top = topNode();

      COLstring Result;
      COLostream ResultStream(Result);

      TRNprintLuaNode(Top, Expanded, ResultStream);

      return Result;
   }
}

COLstring TRNnoxFormatter::view(bool Expanded) const{
   const NODplace& Top = topNode();

   COLstring Result;
   COLostream ResultStream(Result);

   switch (Top.nodeType())
   {
   case NOX_ATTRIBUTE:
      THTMbeginSpan(ResultStream, NOD_SPAN_KEYWORD);
      ResultStream << Top.name() << '=';
      THTMendSpan(ResultStream);
      NOXtreeValue(ResultStream, Top, true);
      break;
   case NOX_ELEMENT:
      if (Expanded){
         THTMbeginSpan(ResultStream, NOD_SPAN_NAME);
         ResultStream << COL_T("&lt;") << Top.name() << COL_T("&gt;");
         THTMendSpan(ResultStream);
      } else {
         NOXtreeValue(ResultStream, Top, true);
      }    
      break;
   case NOX_CDATA:
   case NOX_TEXT:
      NOXtreeValue(ResultStream, Top, true);
      break;
   default:
      ResultStream << Top.name();
      break;
   }

   return Result;
}

COLstring TRNnodbFormatter::view(bool Expanded) const{
   const NODplace& Top = topNode();

   COLstring Result;
   COLostream ResultStream(Result);

   switch (Top.nodeType()){
   case NODB_RESULT_SET:
      ResultStream << COL_T("Result Set");
      TRNprintCountAndUnits(ResultStream, Top.size(), "Row");
      break;
   case NODB_ROW:
      ResultStream << COL_T("Row");
      if (m_TopIndex != -1) ResultStream << ' ' << (m_TopIndex+1);
      if (!Expanded){
         THTMbeginSpan(ResultStream, NOD_SPAN_LIGHT);
         ResultStream << COL_T(" (");
         NTVtreeValue(ResultStream, Top, true);
         ResultStream << ')';
         THTMendSpan(ResultStream); // end NOD_SPAN_LIGHT
      }
      break;
   default: // NODB_VALUE_STRING, NODB_VALUE_INTEGER, NODB_VALUE_DOUBLE, or NODB_VALUE_DATETIME
      THTMprintEscaped(ResultStream, Top.name().c_str(), Top.name().size());
      THTMbeginSpan(ResultStream, NOD_SPAN_LIGHT);
      ResultStream << COL_T(" (");
      NODBprintColumnValue(ResultStream, Top);
      ResultStream << ')';
      THTMendSpan(ResultStream); // end NOD_SPAN_LIGHT
      break;
   }

   return Result;
}

COLstring TRNntabFormatter::view(bool Expanded) const{
   COL_METHOD(TRNntabFormatter::view);
   const NODplace& Top = topNode();
   COL_VAR(Top);
   switch (Top.nodeType()){
      case NTAB_ROOT:{
         COLstring EscapedName;
         COLostream EscapedNameStream(EscapedName);
         THTMprintEscaped(EscapedNameStream, Top.name().c_str(), Top.name().size());
         return EscapedName;
      }
      case NTAB_TABLE_ROW:{
         COLstring Result;
         COLostream ResultStream(Result);
         ResultStream << COL_T("Row");
         if (m_TopIndex != -1) ResultStream << ' ' << (m_TopIndex+1);
         if (!Expanded){
            THTMbeginSpan(ResultStream, NOD_SPAN_LIGHT);
            ResultStream << COL_T(" (");
            NTVtreeValue(ResultStream, Top, true);
            ResultStream << ')';
            THTMendSpan(ResultStream); // end NOD_SPAN_LIGHT
         }
         return Result;
      }
      case NTAB_TABLE:{
         COLstring Result;
         COLostream ResultStream(Result);
         THTMprintEscaped(ResultStream, Top.name().c_str(), Top.name().size());
         TRNprintCountAndUnits(ResultStream, Top.size(), "Row");
         return Result;
      }
      default:{ // NTAB_STRING, NTAB_INTEGER, NTAB_DOUBLE, or NTAB_DATETIME
         COLstring Result;
         COLostream ResultStream(Result);
         THTMprintEscaped(ResultStream, Top.name().c_str(), Top.name().size());
         THTMbeginSpan(ResultStream, NOD_SPAN_LIGHT);
         ResultStream << COL_T(" (");
         NTABprintColumnValue(ResultStream, Top, true);
         ResultStream << ')';
         THTMendSpan(ResultStream); // end NOD_SPAN_LIGHT
         return Result;
      }
   }
}

COLauto<TRNtreeFormatter> TRNgetFormatter(const NODplace& Place){
   // "Complex Lua nodes" can fake their protocol types, for display purposes.
   if (Place.protocolType() == LUAL_LUA_PROTOCOL || NOBisComplexLuaNode(Place)){
      return new TRNluaFormatter();
   }
   switch (Place.protocolType()){
   case 101:   // TODO TODO TODO MAJIK NUMBER
      COL_TRC("HL7 PROTOCOL");
      return new TRNhl7Formatter();
   case NOX_XML:
      return new TRNnoxFormatter();
   case NODB_DB:
      return new TRNnodbFormatter();
   case NTAB_TABLE_PROTOCOL:
      return new TRNntabFormatter();
   default:
      COLcout << "Unexpected NOD protocol type: " << Place.protocolType() << newline;
      COLPRECONDITION(1==0);
      return NULL;
   }
}


