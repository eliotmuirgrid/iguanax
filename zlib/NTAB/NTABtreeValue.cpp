//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABtreeValue
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, November 16th, 2010 @ 01:46:54 PM
//
//---------------------------------------------------------------------------
#include "NTABprecomp.h"
#include "NTABtreeValue.h"
#include <NOD/NODplace.h>
#include <NTAB/NTABconstant.h>
#include <THTM/THTMprinter.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void NTABtableTreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint){
   if (Place.isNull()){
      COL_TRC("Null table group so stream nothing.");
      return;
   }
   Stream << '{';
   int CountOfRow = Place.size();
   for (int i = 0; i < CountOfRow; i++){
      if (i) Stream << COL_T("}, {");
      NTABtreeValue(Stream, Place.child(i), TRNhtmlPrint);
   }
   Stream << '}';
   COL_VAR(CountOfRow);
}

static void NTABtableCollectionTreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint){
   int CountOfSubGrammar = Place.size();
   for (int i = 0; i < CountOfSubGrammar; i++){
      if (i) Stream << COL_T(", ");
      const NODplace& Child = Place.child(i);
      if (TRNhtmlPrint){
         THTMbeginSpan(Stream, NOD_SPAN_NAME);
         THTMprintEscaped(Stream, Child.name().c_str(), Child.name().size());
         THTMendSpan(Stream);
      } else {
         Stream << Child.name();
      }
      Stream << COL_T(":[");
      NTABtreeValue(Stream, Child, TRNhtmlPrint);
      Stream << ']';
   }
}

static void NTABtableRowTreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint){
   // Output row as a CSV
   int CountOfColumn = Place.size();
   for (int i = 0; i < CountOfColumn; i++){
      if (i) Stream << COL_T(", ");
      const NODplace& Child = Place.child(i);
      if (TRNhtmlPrint){
         NTABprintColumnValue(Stream, Child, TRNhtmlPrint);
      } else {
         if (Child.isNull()){
            Stream << COL_T("NULL");
         } else {
            const COLstring& ChildValue = Child.value();
            switch (Child.nodeType()){
               case NTAB_STRING:
               case NTAB_DATETIME:
                  Stream << '\'' << ChildValue << '\'';
                  break;
               case NTAB_INTEGER:
               case NTAB_DOUBLE:
                  Stream << ChildValue;
                  break;
               default:
                  COL_ERROR_STREAM("Unrecognized NTAB value type: " << Child.nodeType(), COLerror::PreCondition);
            }
         }
      }
   }
}

void NTABtreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint){
   COLASSERT(Place.protocolType() == NTAB_TABLE_PROTOCOL);
   switch (Place.nodeType()){
      case NTAB_TABLE:
         NTABtableTreeValue(Stream, Place, TRNhtmlPrint);
         break;
      case NTAB_ROOT:
         NTABtableCollectionTreeValue(Stream, Place, TRNhtmlPrint);
         break;
      case NTAB_TABLE_ROW:
         NTABtableRowTreeValue(Stream, Place, TRNhtmlPrint);
         break;
      default:{ // all value nodes
         if (TRNhtmlPrint){
            NTABprintColumnValue(Stream, Place, TRNhtmlPrint);
         } else {
            if (Place.isNull()){
               Stream << COL_T("NULL");
            } else {
               Stream << Place.value();
            }
         }
      }
   }
}

void NTABprintColumnValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint){
   // COLPRECONDITION(TRNhtmlPrint);
   if (Place.isNull()){
      THTMbeginSpan(Stream, NOD_SPAN_KEYWORD);
      Stream << COL_T("NULL"); // no need to escape
      THTMendSpan(Stream);
   } else {
      const COLstring& ChildValue = Place.value();
      switch (Place.nodeType()){
         case NTAB_STRING:
         case NTAB_DATETIME:
            THTMbeginSpan(Stream, NOD_SPAN_STRING);
            Stream << '\'';
            THTMprintEscapedLiteral(Stream, ChildValue.c_str(), ChildValue.size());
            Stream << '\'';
            THTMendSpan(Stream);
            break;
         case NTAB_INTEGER:
         case NTAB_DOUBLE:
            THTMbeginSpan(Stream, NOD_SPAN_NUMBER);
            Stream << ChildValue;
            THTMendSpan(Stream);
            break;
         default:
            COL_ERROR_STREAM("Unrecognized NTAB value type: " << Place.nodeType(), COLerror::PreCondition);
      }
   }
}
