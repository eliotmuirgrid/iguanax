// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNhl7Formatter
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 09 October 2022 - 05:05PM
// ---------------------------------------------------------------------------

#include <TRN/TRNhl7Formatter.h>
#include <THTM/THTMprinter.h>
#include <LHL7/LHL7node.h>
#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>
#include <NTV/NTVtreeValue.h>
#include <NOB/NOBcomposite.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring TRNhl7Formatter::view(bool Expanded) const{
   COL_METHOD(TRNhl7Formatter::view);
   COLstring Result;
   const NODplace& Node = topNode();
   COL_VAR(Node);
   COLostream ResultStream(Result);
   formatTree(ResultStream, Node, Expanded);
   return Result;
}

void TRNhl7Formatter::formatTree(COLostream& ResultStream, const NODplace& Node, bool Expanded) const{
   COL_METHOD(TRNhl7Formatter::formatTree);
   switch (Node.nodeType()){
   case EDIN_FIELD:
   case EDIN_SUBFIELD:
   case EDIN_SUBSUBFIELD:
      {
         const NODplace* pParent = secondFromTopNode();
         if (pParent && pParent->nodeType() == EDIN_FIELD_ARRAY){
            ResultStream << COL_T("Repeat");
            if (m_TopIndex != -1) ResultStream << ' ' << (m_TopIndex+1);
         }else {
            if (m_TopIndex != -1) ResultStream << (m_TopIndex+1) << ' ';
            THTMprintEscaped(ResultStream, Node.name().c_str(), Node.name().size());
         }
         if (!Expanded && !Node.isNull()){
            THTMbeginSpan(ResultStream, NOD_SPAN_LIGHT);
            ResultStream << COL_T(" (");
            NTVtreeValue(ResultStream, Node, true);
            ResultStream << ')';
            THTMendSpan(ResultStream); // end NOD_SPAN_LIGHT
         }
      }
      break;
   case EDIN_FIELD_ARRAY:
      {
         // Repeated field (eg, "Name (3 Repeats)")
         if (m_TopIndex != -1) ResultStream << (m_TopIndex+1) << ' ';
         THTMprintEscaped(ResultStream, Node.name().c_str(), Node.name().size());
         TRNprintCountAndUnits(ResultStream, Node.size(), "Repeat");
         if (!Expanded && !Node.isNull()){
            ResultStream << COL_T(" - ");
            THTMbeginSpan(ResultStream, NOD_SPAN_LIGHT);
            ResultStream << '(';
            NTVtreeValue(ResultStream, Node, true);
            ResultStream << ')';
            THTMendSpan(ResultStream); // end NOD_SPAN_LIGHT
         }
      }
      break;
   case EDIN_SEGMENT:
      {
         const NODplace* pParent = secondFromTopNode();
         // A specific repeat (eg, "Repeat 1")
         if (pParent && pParent->nodeType() == EDIN_SEGMENT_ARRAY) {
            ResultStream << COL_T("Repeat");
            if (m_TopIndex != -1) ResultStream << ' ' << (m_TopIndex+1);
            if (!Expanded && !Node.isNull()) ResultStream << COL_T(" - ");
         }
         // Normal segment (expanded)
         else if (Expanded || Node.isNull()) { THTMprintEscaped(ResultStream, Node.name().c_str(), Node.name().size()); }

         if (!Expanded && !Node.isNull())    { NTVtreeValue(ResultStream, Node, true); }
      }
      break;
   case EDIN_SEGMENT_ARRAY:
      {
         // Repeated segment (eg, "OBX (3 Repeats)")
         THTMprintEscaped(ResultStream, Node.name().c_str(), Node.name().size());
         TRNprintCountAndUnits(ResultStream, Node.size(), "Repeat");
         if (!Expanded && !Node.isNull()){ 
            ResultStream << COL_T(" - ");
            NTVtreeValue(ResultStream, Node, true);
         }
      }
      break;
   case EDIN_SEGMENT_GROUP:
      {
         const NODplace* pParent = secondFromTopNode();
         // A specific repeat (eg, "Repeat 1")
         if (pParent && pParent->nodeType() == EDIN_SEGMENT_GROUP_ARRAY) {
            ResultStream << COL_T("Repeat");
            if (m_TopIndex != -1) ResultStream << ' ' << (m_TopIndex+1);
         }
         // Normal segment group
         else { THTMprintEscaped(ResultStream, Node.name().c_str(), Node.name().size()); }
         if (!Expanded && !Node.isNull()){
            ResultStream << COL_T(" - ");
            NTVtreeValue(ResultStream, Node, true);
         }
      }
      break;
   case EDIN_SEGMENT_GROUP_ARRAY:
      {
         // Repeated segment group (eg, "Insurance (3 Repeats)")
         THTMprintEscaped(ResultStream, Node.name().c_str(), Node.name().size());
         TRNprintCountAndUnits(ResultStream, Node.size(), "Repeat");
         if (!Expanded && !Node.isNull()){
            ResultStream << COL_T(" - ");
            NTVtreeValue(ResultStream, Node, true);
         }
      }
      break;
   case EDIN_MESSAGE:
      THTMprintEscaped(ResultStream, Node.name().c_str(), Node.name().size());
      break;
   }
}
