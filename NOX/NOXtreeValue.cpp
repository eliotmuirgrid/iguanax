//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXtreeValue
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, November 9th, 2010 @ 03:15:34 PM
//
//---------------------------------------------------------------------------
#include "NOXprecomp.h"
#pragma hdrstop

#include "NOXtreeValue.h"
#include "NOXconstant.h"
#include <THTM/THTMprinter.h>
#include <NOD/NODplace.h>
#include <NOD/NODprintCallback.h>

#include <SFI/SFIxmlDataEncodeFilter.h>
#include <SFI/SFIxmlAttributeDataEncodeFilter.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void NOXdocumentTreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint){
   int CountOfChild = Place.size();
   for (int i = 0; i < CountOfChild; i++)
   {
      if (i){
         if (TRNhtmlPrint){
            Stream << ' ';
         } else {
            Stream << newline;
         }
      }
      NOXtreeValue(Stream, Place.child(i), TRNhtmlPrint);
   }
}

static void NOXelementTreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint){
   if (TRNhtmlPrint){
      THTMbeginSpan(Stream, NOD_SPAN_NAME);
      Stream << COL_T("&lt;") << Place.name(); // Element names shouldn't contain characters that need escaping.
      THTMendSpan(Stream);
   } else {
      Stream << '<' << Place.name();
   }

   int CountOfChild = Place.size();

   // Attributes first
   int AttributeIndex = 0;
   while (AttributeIndex < CountOfChild && Place.child(AttributeIndex).nodeType() == NOX_ATTRIBUTE){
      const NODplace& AttributeNode = Place.child(AttributeIndex++);
      Stream << ' ';
      if (TRNhtmlPrint){
         THTMbeginSpan(Stream, NOD_SPAN_KEYWORD);
         Stream << AttributeNode.name() << '='; // Attribute names shouldn't contain characters that need escaping.
         THTMendSpan(Stream);
         NOXtreeValue(Stream, AttributeNode, TRNhtmlPrint);
      } else {
         Stream << AttributeNode.name() << COL_T("=\"");
         NOXtreeValue(Stream, AttributeNode, TRNhtmlPrint);
         Stream << '\"';
      }
   }

   if (TRNhtmlPrint){
      THTMbeginSpan(Stream, NOD_SPAN_NAME);
      Stream << COL_T("&gt;");
      THTMendSpan(Stream);
   } else {
      Stream << '>';
   }
   
   // Then child elements
   int i=AttributeIndex, t;
   if(i >= CountOfChild) {
      // No children, omit newline/indentation.
   } else if(i+1 == CountOfChild && (t = Place.child(i).nodeType(), NOX_TEXT == t || NOX_CDATA == t)) {
      // Only one child, text or CDATA, so drop in without newlines/indentation.
      NOXtreeValue(Stream, Place.child(i), TRNhtmlPrint);
   } else {
      if (!TRNhtmlPrint) Stream << indent;
      for (int ChildIndex = AttributeIndex; ChildIndex < CountOfChild; ChildIndex++)
      {
         if (TRNhtmlPrint){
            Stream << ' ';
         } else {
            Stream << newline;
         }
         NOXtreeValue(Stream, Place.child(ChildIndex), TRNhtmlPrint);
      }
      if (TRNhtmlPrint){
         Stream << ' ';
      } else {
         Stream << unindent << newline;
      }
   }

   if (TRNhtmlPrint){
      THTMbeginSpan(Stream, NOD_SPAN_NAME);
      Stream << COL_T("&lt;/") << Place.name() << COL_T("&gt;"); // Element names shouldn't contain characters that need escaping.
      THTMendSpan(Stream);
   } else {
      Stream << COL_T("</") << Place.name() << '>';
   }
}

void NOXtreeValue(COLostream& Stream, const NODplace& Place, bool TRNhtmlPrint){
   COLASSERT(Place.protocolType() == NOX_XML);
   const char* pOriginalNewline = Stream.getNewLine();
   Stream.setNewLine("\n");

   switch (Place.nodeType()){
   case NOX_DOCUMENT: NOXdocumentTreeValue(Stream, Place, TRNhtmlPrint); break;
   case NOX_ELEMENT:  NOXelementTreeValue( Stream, Place, TRNhtmlPrint); break;
   case NOX_ATTRIBUTE:{
      COLstring EscapedValue;
      SFIxmlAttributeDataEncode(Place.value(), &EscapedValue);
      if (TRNhtmlPrint){
         THTMbeginSpan(Stream, NOD_SPAN_STRING);
         Stream << COL_T("&quot;");
         THTMprintEscaped(Stream, EscapedValue.c_str(), EscapedValue.size());
         Stream << COL_T("&quot;");
         THTMendSpan(Stream);
      } else {
         Stream << EscapedValue;
      }
   } break;
   case NOX_TEXT:{
      COLstring EscapedValue;
      SFIxmlDataEncode(Place.value(), &EscapedValue);
      if (TRNhtmlPrint){
         THTMbeginSpan(Stream, NOD_SPAN_TEXT);
         THTMprintEscaped(Stream, EscapedValue.c_str(), EscapedValue.size());
         THTMendSpan(Stream);
      } else {
         Stream << EscapedValue;
      }
   } break;
   case NOX_CDATA:{
      COLstring CdataString = "<![CDATA[" + Place.value() + "]]>";
      if (TRNhtmlPrint){
         THTMbeginSpan(Stream, NOD_SPAN_TEXT);
         THTMprintEscaped(Stream, CdataString.c_str(), CdataString.size());
         THTMendSpan(Stream);
      } else {
         Stream << CdataString;
      }
   } break;
   default:
      COLASSERT(false);
   }

   Stream.setNewLine(pOriginalNewline);
}
