//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODBtreeValue
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, November 9th, 2010 @ 04:31:43 PM
//
//---------------------------------------------------------------------------
#include "NODBprecomp.h"
#pragma hdrstop

#include "NODBtreeValue.h"
#include "NODBconstant.h"

#include <NOD/NODplace.h>
#include <NOD/NODprintCallback.h>
#include <THTM/THTMprinter.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void NODBrowTreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint){
   COLuint32 CountOfColumn = Place.size();
   for (COLuint32 ColumnIndex = 0; ColumnIndex < CountOfColumn; ColumnIndex++){
      if (ColumnIndex) Stream << COL_T(", ");
      const NODplace& ColumnValue = Place.child(ColumnIndex);
      if (TRNhtmlPrint){
         NODBtreeValue(Stream, ColumnValue, TRNhtmlPrint);
      } else {
         if (ColumnValue.isNull()){
            Stream << COL_T("NULL");
         } else {
            switch (ColumnValue.nodeType())
            {
            case NODB_VALUE_STRING:
            case NODB_VALUE_DATETIME:
               Stream << '\'' << ColumnValue.value() << '\'';
               break;
            default:
               Stream << ColumnValue.value();
               break;
            }
         }
      }
   }
}

void NODBtreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint){
   COLASSERT(Place.protocolType() == NODB_DB);
   switch(Place.nodeType()){
      case NODB_RESULT_SET:{
         int CountOfRow = Place.size();
         for (int Row = 0; Row < CountOfRow; Row++)
         {
            if (Row) Stream << COL_T(", ");
            Stream << '{';
            NODBtreeValue(Stream, Place.child(Row), TRNhtmlPrint);
            Stream << '}';
         }
      } break;
      case NODB_ROW:
         NODBrowTreeValue(Stream, Place, TRNhtmlPrint);
         break;
      default:{ // All value nodes
         if (!TRNhtmlPrint){
            if (Place.isNull()) { Stream << COL_T("NULL"); } else { Stream << Place.value(); }
         } else { // pCallback
            NODBprintColumnValue(Stream, Place);
         }
      }
   }
}

void NODBprintColumnValue(COLostream& Stream, const NODplace& Place){
   COL_FUNCTION(NODBprintColumnValue);
   if (Place.isNull()){
      THTMbeginSpan(Stream, NOD_SPAN_KEYWORD);
      Stream << COL_T("NULL");
      THTMendSpan(Stream);
   } else {
      switch (Place.nodeType()){
         case NODB_VALUE_STRING:
         case NODB_VALUE_DATETIME:
         case NODB_VALUE_BINARY:
            THTMbeginSpan(Stream, NOD_SPAN_STRING);
            Stream << '\'';
            THTMprintEscapedLiteral(Stream, Place.value().c_str(), Place.value().size());
            Stream << '\'';
            THTMendSpan(Stream);
            break;
         case NODB_VALUE_INTEGER:
         case NODB_VALUE_DOUBLE:
            THTMbeginSpan(Stream, NOD_SPAN_NUMBER);
            THTMprintEscapedLiteral(Stream, Place.value().c_str(), Place.value().size());
            THTMendSpan(Stream);
            break;
         default:
            COL_ERROR_STREAM("Unrecognized NODB type: " << Place.nodeType(), COLerror::PreCondition);
            break;
      }
   }
}
