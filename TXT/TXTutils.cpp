//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTutils
//
// Description:
//
// Implementation
//
// Author: Eric Mulvaney
// Date:   Friday, May 29th, 2009 @ 11:20:13 AM
//
//---------------------------------------------------------------------------
#include "TXTprecomp.h"
#pragma hdrstop

#include "TXTutils.h"
#include "TXTencoding.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <COL/COLvar.h>
#include <COL/COLstring.h>

#include <ctype.h>

#ifdef _WIN32
#include <windows.h>
#endif

int TXTnaturalCompareStrings(const COLstring& Left, const COLstring& Right) {
   COL_FUNCTION(TXTnaturalCompareStrings);
   const char* pLeft  = (const char*)Left.c_str();
   const char* pRight = (const char*)Right.c_str();

   COLPRECONDITION( pLeft );
   COLPRECONDITION( pRight );

   {
      int CurrentLeft;
      int CurrentRight;

      while( (CurrentLeft = (unsigned char)*pLeft) && (CurrentRight = (unsigned char)*pRight) ) {
         if( isdigit(CurrentLeft) && isdigit(CurrentRight) )  {
            long  LeftNumber = strtol(pLeft,  (char**)&pLeft,  10);
            long RightNumber = strtol(pRight, (char**)&pRight, 10);

            if( LeftNumber != RightNumber ) {
               return LeftNumber - RightNumber;
            }
         } else {
            CurrentLeft  = tolower(CurrentLeft);
            CurrentRight = tolower(CurrentRight);

            if( CurrentLeft != CurrentRight ) {
               return CurrentLeft - CurrentRight;
            }

            ++pLeft;
            ++pRight;
         }
      }
   }

   return (unsigned char)*pLeft - (unsigned char)*pRight;
}


// Returns true if the characters between pBegin and pEnd (exclusive)
// are punctuation and unique.
//
inline int uniqueDelimiters(const char* pBegin, const char* pEnd) {
   COL_FUNCTION(uniqueDelimiters);
   while(pBegin < --pEnd) {
      if (!ispunct(*pEnd) || memchr(pBegin, *pEnd, pEnd - pBegin)) {
         return 0;
      }
   }
   return 1;
}

// findHl7EscapeChar(pIn) returns the escape character when present
// in the standard HL7 message pIn.  Leading whitespace is ignored.
//
static int findHl7EscapeChar(char** ppIn) {
   COL_FUNCTION(findHl7EscapeChar);
   char* pIn = *ppIn;
   struct Head_t {
      char Name[3], Field, Subfield, Repeat, Escape, SubSubfield, FieldAgain;
   } Head;
   pIn += strspn(pIn, "\t\n\v\f\r ");  // Skip whitespace.
   // #24671 - Upgrading valgrind-dell to Ubuntu 12.04 introduced illegal read errors here.
   // Restoring the original strncpy cleared those.
   strncpy((char*)&Head, pIn, sizeof Head); 

   // #24637 - 2 Iguana test cases failing (buffer overflow) on Ubuntu 10.04.4... (valgrind-dell)
   //memcpy((void*)&Head, pIn, 9);

   if(!memchr(&Head, 0, sizeof Head)
      && !memcmp(Head.Name, "MSH", 3)
      && Head.Field == Head.FieldAgain
      && uniqueDelimiters(&Head.Subfield, &Head.FieldAgain + 1)) {
      *ppIn = pIn + sizeof Head;
      return Head.Escape;
   }
   return 0;  // Failed.
}

void TXTunescapeExtendedAscii(COLstring& String) {
   COL_FUNCTION(TXTunescapeExtendedAscii);
   char* pIn = String.get_buffer();
   char EscapeChar[2] = {0};
   EscapeChar[0] = findHl7EscapeChar(&pIn);
   if (EscapeChar[0]) { // Handle only standard HL7 messages.
      char* pOut = pIn;
      size_t Size;

      while ((Size = strcspn(pIn, EscapeChar)), pIn[Size]) {
         int Char;
         if (pIn != pOut)
            memmove(pOut, pIn, Size);
         pOut += Size;
         pIn  += Size;
         if ((pIn[1] == 'x' || pIn[1] == 'X')
            && isxdigit(pIn[2]) && isxdigit(pIn[3]) && (pIn[4] == pIn[0])
            && (Char = strtol(pIn+2, NULL, 16)) >= 0x80) {
            *pOut++ = Char;
            pIn += 5;
         } else {
            *pOut++ = *pIn++; 
         }
      }

      if(pIn != pOut) { // If false, we've changed nothing.
         memmove(pOut, pIn, Size + 1);
         String.setSize(pOut - String.get_buffer() + Size);
      }
   }
}


//
// HL7 escaping of 8-bit characters.
//
// TODO:  It might be a little faster to not use strcspn() for short 8-bit
// sequences, like those in western languages, but I have yet to benchmark
// this code.  For long sequences, as in many eastern languages, it should
// be faster than simple iteration.  (-Eric.)
//

//strspn's documentation specifies that the search ends at but does not include the \0 so this array must be null terminated to prevent overflow
static const char s_Ascii[] = {
         0001, 0002, 0003, 0004, 0005, 0006, 0007,
   0010, 0011, 0012, 0013, 0014, 0015, 0016, 0017,
   0020, 0021, 0022, 0023, 0024, 0025, 0026, 0027,
   0030, 0031, 0032, 0033, 0034, 0035, 0036, 0037,
   0040, 0041, 0042, 0043, 0044, 0045, 0046, 0047,
   0050, 0051, 0052, 0053, 0054, 0055, 0056, 0057,
   0060, 0061, 0062, 0063, 0064, 0065, 0066, 0067,
   0070, 0071, 0072, 0073, 0074, 0075, 0076, 0077,
   0100, 0101, 0102, 0103, 0104, 0105, 0106, 0107,
   0110, 0111, 0112, 0113, 0114, 0115, 0116, 0117,
   0120, 0121, 0122, 0123, 0124, 0125, 0126, 0127,
   0130, 0131, 0132, 0133, 0134, 0135, 0136, 0137,
   0140, 0141, 0142, 0143, 0144, 0145, 0146, 0147,
   0150, 0151, 0152, 0153, 0154, 0155, 0156, 0157,
   0160, 0161, 0162, 0163, 0164, 0165, 0166, 0167,
   0170, 0171, 0172, 0173, 0174, 0175, 0176, 0177, 
   '\0'
};

static size_t count8bit(const char* pIn) {
   COL_FUNCTION(count8bit);
   size_t Count=0, Size;
   while((Size = strspn(pIn, s_Ascii)), pIn[Size]) {
      pIn += Size;
      Size = strcspn(pIn, s_Ascii);
      pIn += Size;
      Count += Size;
   }
   return Count;
}

inline char* writeEscape(char* pOut, unsigned char Byte, char EscapeChar) {
   COL_FUNCTION(writeEscape);
   int Hi = Byte >> 4;
   int Lo = Byte & 0x0f;

   *pOut++ = EscapeChar;
   *pOut++ = 'X';
   *pOut++ = (Hi < 10) ? ('0' + Hi) : ('A' - 10 + Hi);
   *pOut++ = (Lo < 10) ? ('0' + Lo) : ('A' - 10 + Lo);
   *pOut++ = EscapeChar;

   return pOut;
}

void TXTescapeExtendedAscii(COLstring& Input, char EscapeChar) {
   COL_FUNCTION(TXTescapeExtendedAscii);
   char* pIn = Input.get_buffer();
   size_t Size;

   if (!EscapeChar || !(Size = count8bit(pIn))) {
      return;  // No escape character or nothing to escape.
   }
   size_t OutputSize = Input.size() + Size * 4;
   COLstring Output;
   Output.setCapacity(OutputSize + 1);
   char* pOut = Output.get_buffer();

   while((Size = strspn(pIn, s_Ascii)), pIn[Size]) {
      memcpy(pOut, pIn, Size);
      pOut += Size;
      pIn  += Size;
      for(Size = strcspn(pIn, s_Ascii); Size--; pIn++) {
         pOut = writeEscape(pOut, *pIn, EscapeChar);
      }
   }

   memcpy(pOut, pIn, Size + 1);
   COLASSERT(pOut - Output.get_buffer() + Size == OutputSize);
   Output.setSize(OutputSize);

   Input.swap(&Output);
}

void TXTescapeExtendedAscii(COLstring& String) {
   COL_FUNCTION(TXTescapeExtendedAscii);
   char* pIn = String.get_buffer();
   char EscapeChar = findHl7EscapeChar(&pIn);
   if(EscapeChar) {
      TXTescapeExtendedAscii(String, EscapeChar);
   }
}

void TXTescapeNewLines(COLstring* pString) {
   COL_FUNCTION(TXTescapeNewLines);
   pString->replace("\\", "\\\\");
   pString->replace("\r", "\\r");
   pString->replace("\n", "\\n");
}

void TXTunescapeNewLines(COLstring* pString) {
   COL_FUNCTION(TXTunescapeNewLines);
   pString->replace("\r","");
   pString->replace("\n","");
   pString->replace("\\r", "\r");
   pString->replace("\\n", "\n");
   pString->replace("\\\\", "\\");
}

bool TXThasBadAscii(const COLstring& Data) {
   COL_FUNCTION(TXThasBadAscii);
   int DataSize = Data.size();
   for (int i = 0; i < DataSize; i++){
      unsigned char c = Data[i];
      if(iscntrl(c) && !strchr("\r\n\t", c)) {
         COL_TRC("Unprintable control characters found.");
         return true;
      }
   }
   return false;
}

