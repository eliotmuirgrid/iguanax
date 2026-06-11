//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: THMLprinter
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, January 11th, 2012 @ 09:27:05 AM
//---------------------------------------------------------------------------
#include <THTM/THTMprinter.h>

#include <LUAC/lapi.h>
#include <LUA/LUAheader.h>
#include <SFI/SFIhtmlEscape.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void THTMbeginSpan(COLostream& Stream, NODprintSpanType SpanType) {
   COL_FUNCTION(THTMbeginSpan);
   switch (SpanType){
      case NOD_SPAN_KEYWORD: Stream << COL_T("<span    class=\'clrz_kw\'>");    break;
      case NOD_SPAN_STRING:  Stream << COL_T("<span    class=\'clrz_str\'>");   break;
      case NOD_SPAN_NUMBER:  Stream << COL_T("<span    class=\'clrz_num\'>");   break;
      case NOD_SPAN_LIGHT:   Stream << COL_T("<span    class=\'clrz_light\'>"); break;
      case NOD_SPAN_NAME:    Stream << COL_T("<span    class=\'clrz_name\'>");  break;
      case NOD_SPAN_TEXT:    Stream << COL_T("<span    class=\'clrz_text\'>");  break;
		case NOD_SPAN_ERR:     Stream << COL_T("<span    class=\"clrz_err\">");   break;
		default: COL_ERROR_STREAM("Unrecognized span type: " << SpanType, COLerror::PreCondition);
   }
}

void THTMendSpan(COLostream& Stream) {
   COL_FUNCTION(THTMendSpan);
   Stream << COL_T("</span>");
}

static void THTMhexCode(COLostream& Stream, unsigned char Char){
   COL_FUNCTION(THTMhexCode);
   char Buffer[10];
   snprintf(Buffer, 10, "%03d", (int)Char);
   COL_VAR(Buffer);
   COL_VAR3(Buffer[0], Buffer[1], Buffer[2]);
   Stream << COL_T("<span class='ANNcontrolCharColor'>\\") << Buffer[0] << Buffer[1] << Buffer[2] << COL_T("</span>");
}

static void THTMluaReplaceControlChar(COLostream& Stream, unsigned char Char) {
   COL_FUNCTION(THTMluaReplaceControlChar);
   switch (Char) {
      case '\"': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\\"</span>"); break;
      case '\'': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\\'</span>"); break;
      case '\\': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\\\</span>"); break;
      case '\a': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\a</span>"); break;
      case '\b': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\b</span>"); break;
      case '\f': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\f</span>"); break;
      case '\n': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\n</span>"); break;
      case '\r': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\r</span>"); break;
      case '\t': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\t</span>"); break;
      case '\v': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\v</span>"); break;
      default  :
         THTMhexCode(Stream, Char);
         break;
   }
}

static void THTMluaReplaceKeepNewlines(COLostream& Stream, unsigned char Char) {
   COL_FUNCTION(THTMluaReplaceKeepNewlines);
   switch (Char) {
      case '\"': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\\"</span>"); break;
      case '\'': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\\'</span>"); break;
      case '\\': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\\\</span>"); break;
      case '\a': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\a</span>"); break;
      case '\b': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\b</span>"); break;
      case '\f': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\f</span>"); break;
      case '\n': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\n</span>\n"); break;
      case '\r': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\r</span>\n"); break;
      case '\t': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\t</span>"); break;
      case '\v': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\v</span>"); break;
      default  :
         THTMhexCode(Stream, Char);
         break;
   }
}

// Control characters only.
static void THTMluaReplaceControlOnly(COLostream& Stream, unsigned char Char) {
   COL_FUNCTION(THTMluaReplaceControlOnly);
   switch (Char) {
      case '\a': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\a</span>"); break;
      case '\b': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\b</span>"); break;
      case '\f': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\f</span>"); break;
      case '\n': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\n</span>"); break;
      case '\r': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\r</span>"); break;
      case '\t': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\t</span>"); break;
      case '\v': Stream << COL_T("<span  class='ANNcontrolCharColor' >\\v</span>"); break;
      default  :
         THTMhexCode(Stream, Char);
         break;
   }
}

bool THTMisControlChar(unsigned int Char){
   //COL_FUNCTION(THTMisControlChar);
   //COL_VAR(Char);
   if (Char < 0x20 || Char >= 0x7F){
      return true;
   }
   return false;
}

// This detects ', ", \ as well as characters which are out of the range of 0x20 --> 0x7E
bool THTMisSpecial(unsigned int Char){
   //COL_FUNCTION(THTMisSpecial);
   //COL_VAR(Char);
   switch(Char){
      case '\'':
      case '\\':
      case '\"':
         //COL_TRC("It's a special case!");
         return true;
         break;
      default:
         if (Char < 0x20 || Char >= 0x7F){
            //COL_TRC("It's special!");
            return true;
         }
   }
   //COL_TRC("Not special.");
   return false;
}


int THTMutf8CharWidth(const char* pChar){
   if (*pChar >= 0){
      return 1;  // signed char so it's in the range from 0-127
   }
   // we have the beginning of a Utf8 character
   int i = 1;
   while ((i < 7) && ((pChar[i] & 0xC0) == 0x80)){
      i++;
   }
   if (i == 7){
      // TODO - more attention required here.
      return 1;
   }
   return i;
}

void THTMprintEscaped(COLostream& Stream, const char* pData, int Length){
   COL_FUNCTION(THTMprintEscaped);
   COL_VAR(Length);
   const char* pLastMatch = pData; // first character after last match
   const char* pIterator = pData;
   const char* pEnd = pData + Length;
   while(pIterator < pEnd){
      int Increment = THTMutf8CharWidth(pIterator);
      COL_VAR(Increment);
      if (Increment==1 && THTMisControlChar(*pIterator)){
         SFIhtmlEscape(pLastMatch, pIterator-pLastMatch, *Stream.sink());
         THTMluaReplaceControlOnly(Stream, *pIterator);
         pIterator++;
         pLastMatch = pIterator;
      } else {
         pIterator += Increment;
      }
   }
   SFIhtmlEscape(pLastMatch, Length-(pLastMatch-pData), *Stream.sink());
}

void THTMprintEscapedLiteral(COLostream& Stream, const char* pData, int Length){
   COL_FUNCTION(THTMprintEscapedLiteral);
   const char* pLastMatch = pData; // first character after last match
   const char* pIterator = pData;
   const char* pEnd = pData + Length;

   while (pIterator < pEnd){
      int Increment = THTMutf8CharWidth(pIterator);
      if (Increment == 1 && THTMisSpecial(*pIterator)){
         SFIhtmlEscapeNoNewline(pLastMatch, pIterator-pLastMatch, *Stream.sink());
         THTMluaReplaceControlChar(Stream, *pIterator);
         pIterator++;
         pLastMatch = pIterator;
      } else {
         pIterator+=Increment;
      }
   }
   SFIhtmlEscapeNoNewline(pLastMatch, pIterator-pLastMatch, *Stream.sink());
}

void THTMprintEscapedLiteralWithNewlines(COLostream& Stream, const char* pData, int Length){
   COL_FUNCTION(THTMprintEscapedLiteralWithNewlines);
   const char* pLastMatch = pData; // first character after last match
   const char* pIterator = pData;
   const char* pEnd = pData + Length;

   while (pIterator < pEnd){
      int Increment = THTMutf8CharWidth(pIterator);
      if (Increment == 1 && THTMisSpecial(*pIterator)){
         SFIhtmlEscapeNoNewline(pLastMatch, pIterator-pLastMatch, *Stream.sink());
         THTMluaReplaceKeepNewlines(Stream, *pIterator);
         pIterator++;
         pLastMatch = pIterator;
      } else {
         pIterator += Increment;
      }
   }
   SFIhtmlEscapeNoNewline(pLastMatch, pIterator-pLastMatch, *Stream.sink());
}
