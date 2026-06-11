//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTVhl7TreeValue
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, November 10th, 2010 @ 03:20:50 PM
//
//---------------------------------------------------------------------------
#include <NOD/NODplace.h>
#include <NOD/NODprintCallback.h>

#include <THTM/THTMprinter.h>
#include <EDI/EDIdata.h>
#include <EDI/EDIformat.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void NTVhl7TreeValueImpl(COLostream& Stream, const NODplace& Place, bool PrintHtml);

static inline void NTVhl7PrintDelimiter(COLostream& Stream, char Delimiter, bool PrintHtml){
   if (PrintHtml){
      THTMbeginSpan(Stream, NOD_SPAN_KEYWORD);
      THTMprintEscaped(Stream, &Delimiter, 1);
      THTMendSpan(Stream);
   } else {
      Stream << Delimiter;
   }
}

// StartAtMember should only be non-zero when generating the flatwire for an MSH segment.
// This is because we don't put a field separator between the first and second fields.
// SkipNullMembers will not print extra separator characters for missing fields.  It is
// intended to be used at the "Segment" level, where we do not wish to output empty lines.
//
static void NTVhl7StreamChildren(
   COLostream& Stream,
   const NODplace& Place,
   char SeparatorCharacter,
   bool PrintHtml,
   int StartAtChild = 0,
   bool SkipNullMembers = false
){
   if (!Place.isNull()){
      int CountOfChild = Place.size();
      int SkippedDelimiters = 0; // only output delimiters if there's going to be a value after them.

      for (int ChildIndex = StartAtChild; ChildIndex < CountOfChild; ++ChildIndex){
         if (ChildIndex) SkippedDelimiters++;
         const NODplace& Child = Place.child(ChildIndex);
         if (!Child.isNull()){
            for (int i = 0; i < SkippedDelimiters; i++){
               NTVhl7PrintDelimiter(Stream, SeparatorCharacter, PrintHtml);
            }
            SkippedDelimiters = 0;
            NTVhl7TreeValueImpl(Stream, Child, PrintHtml);
         }
         else if (SkipNullMembers){
            SkippedDelimiters--;
         }
      }
   }
}

static void NTVhl7StreamSimpleValue(COLostream& Stream, const NODplace& Place, bool PrintHtml){
   if (Place.isNull() || Place.value().is_null()) return;
   COL_VAR(Place.nodeTypeName());
   COL_VAR2(PrintHtml, Place.shouldEscapeValue());
   if (Place.shouldEscapeValue()){
      COLstring Result;
      EDIescapeData(&Result, Place.value());
      COL_VAR2(Place.value(), Result);
      if (PrintHtml){
         THTMbeginSpan(Stream, NOD_SPAN_STRING);
         THTMprintEscaped(Stream, Result.c_str(), Result.size());
         THTMendSpan(Stream);
      } else {
         Stream << Result;
      }
   } else {
      if (PrintHtml){
         THTMbeginSpan(Stream, NOD_SPAN_STRING);
         THTMprintEscaped(Stream, Place.value().c_str(), Place.value().size());
         THTMendSpan(Stream);
      } else {
         Stream << Place.value();
      }
   }
}

static void NTVhl7SegmentTreeValue(COLostream& Stream, const NODplace& Place, bool PrintHtml){
   char FieldSeparatorChar = '|';

   if (PrintHtml){
      THTMbeginSpan(Stream, NOD_SPAN_NAME);
      THTMprintEscaped(Stream, Place.name().c_str(), Place.name().size());
      THTMendSpan(Stream);
   } else {
      Stream << Place.name();
   }

   int StartAtField = 0;
   if (Place.name() == "MSH"){
      Stream << "|^~\\&";
      StartAtField = 2;
   } else {
      NTVhl7PrintDelimiter(Stream, FieldSeparatorChar, PrintHtml);
   }

   NTVhl7StreamChildren(Stream, Place, FieldSeparatorChar, PrintHtml, StartAtField);
   // // Output trailing segment delimiter (not strictly necessary - do it to match Iguana 6)
   NTVhl7PrintDelimiter(Stream, FieldSeparatorChar, PrintHtml); 
}

static void NTVhl7TreeValueImpl(COLostream& Stream, const NODplace& Place, bool PrintHtml){
   COLASSERT(Place.protocolType() == 101); // TODO- Majik string);
   switch (Place.nodeType()){
      case EDIN_SEGMENT:
         NTVhl7SegmentTreeValue(Stream, Place, PrintHtml);
         break;
      case EDIN_SEGMENT_ARRAY:
      case EDIN_SEGMENT_GROUP:
      case EDIN_SEGMENT_GROUP_ARRAY:
      case EDIN_MESSAGE:
         NTVhl7StreamChildren(Stream, Place, '\r', PrintHtml, 0, true);
         // Last segment won't have a delimiter after it (#18929).
         if (Place.nodeType() == EDIN_MESSAGE){ // Important! Without this check, things like repeating segments and segment groups introduce extra newlines IX-3453
            NTVhl7PrintDelimiter(Stream, '\r', PrintHtml);
         }
         break;
      case EDIN_FIELD:
         switch (Place.grammarType()){
            case NOD_SIMPLE: // Leaf field
               NTVhl7StreamSimpleValue(Stream, Place, PrintHtml);
               break;
            case NOD_COMPLEX: // Complex field
               NTVhl7StreamChildren(Stream, Place, '^', PrintHtml);
               break;
            default: COLASSERT(false);
         }
         break;
      case EDIN_FIELD_ARRAY:
         NTVhl7StreamChildren(Stream, Place, '~', PrintHtml);
         break;
      case EDIN_SUBFIELD:
         if (Place.isSimple()){ // Leaf subfield
            NTVhl7StreamSimpleValue(Stream, Place, PrintHtml);
         } else { // Complex subfield
            NTVhl7StreamChildren(Stream, Place, '&', PrintHtml);
         }
         break;
      case EDIN_SUBSUBFIELD:
         COLASSERT(Place.isSimple());
         NTVhl7StreamSimpleValue(Stream, Place, PrintHtml);
   }
}

void NTVhl7TreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint){
   COL_FUNCTION(NTVhl7TreeValue);
   NTVhl7TreeValueImpl(Stream, Place, TRNhtmlPrint);
}
