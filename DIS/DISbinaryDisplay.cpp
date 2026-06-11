//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DISbinaryDisplay
//
// Description:
//
// This object nicely formats binary data in an HTML format.
//
// Author: Eliot Muir
// Date:   Sun 09/29/2002 
//
//---------------------------------------------------------------------------
#include "DISprecomp.h"
#pragma hdrstop

#include <DIS/DISbinaryDisplay.h>
#include <COL/COLerror.h>
#include <COL/COLstring.h>
#include <COL/COLostream.h>
#include <COL/COLsink.h>

#include <ctype.h>

class DISbinaryDisplayPrivate
{
public:
   static bool doHtmlFormat(COLostream& DumpStream, unsigned char Character, bool AsHtml, int ErrorIndex, int CharIndex);
   static void toChar(COLostream& DumpStream, unsigned char Character, bool AsHtml, int ErrorIndex, int CharIndex);
   static int numberLength(int Value);
   static void toHex(COLostream& DumpStream, unsigned char Character, bool AsHtml, int ErrorIndex, int CharIndex);

private:
   DISbinaryDisplayPrivate(const DISbinaryDisplayPrivate& Orig);
   DISbinaryDisplayPrivate& operator=(const DISbinaryDisplayPrivate& Orig);
};

class DISbinaryDisplayLookups
{
public:
   DISbinaryDisplayLookups();

   //map from a character to a given html font. 0 if no font needed
   const char* HtmlFont[256];
   //map from a character to printable version
   char CharPrintable[256];
   //map from a character to the html version
   COLstring HtmlCharacter[256];
};

DISbinaryDisplayLookups::DISbinaryDisplayLookups()
{
   COLuint8 Char = 0;
   do 
   {
      if(!isprint(Char))
      {
         if (Char == '\r') HtmlFont['\r'] = "<font class=\"hst\">";  // Not sure what "st" stands for.  :-)
         else if (iscntrl(Char)) HtmlFont[Char] = "<font class=\"hctrl\">"; // Other than \r (handled above).
         else HtmlFont[Char] = "<font class=\"hnasc\">";// Non-ASCII.
      }
      else
      {
         HtmlFont[Char] = 0;
      }
      if ((Char < 0x20) || (Char > 0x7F))
      {
         CharPrintable[Char] = '.';
         HtmlCharacter[Char] = ".";
      }
      else
      {
         CharPrintable[Char] = Char;
         switch(Char)
         {
         case '<': HtmlCharacter[Char] = "&lt;";  break;
         case '>': HtmlCharacter[Char] = "&gt;";  break;
         case '&': HtmlCharacter[Char] = "&amp;"; break;
         default:
            HtmlCharacter[Char].write(&Char,1);
         }
      }
   }
   while(++Char);
}

static const DISbinaryDisplayLookups DISlookups;

//for convenient space streaming
class DISspace
{
public:
   DISspace() : SpaceCount(1){}
   DISspace(COLindex iSpaceCount) : SpaceCount(iSpaceCount)
   {
      COLPRECONDITION(SpaceCount >= 0);
   }
   ~DISspace(){}

   COLindex spaceCount() const
   {
      return SpaceCount;
   }
private:
   COLindex SpaceCount;
};

COLostream& operator<<(COLostream& Stream, const DISspace& Space)
{
   COLindex Total = Space.spaceCount();
   while(Total--)
   {
      Stream << "&nbsp;";
   }
   return Stream;
}

inline bool DISbinaryDisplayPrivate::doHtmlFormat(COLostream& DumpStream, unsigned char Character, bool AsHtml, int ErrorIndex, int CharIndex)
{
   if (AsHtml)
   {
      if (ErrorIndex == CharIndex)
      {
         DumpStream << "<font class=\"herr\">";
         return true;
      }
      else if( const char* pHtml = DISlookups.HtmlFont[Character] )
      {
         DumpStream << pHtml;
         return true;
      }
   }   
   return false;
}


void DISbinaryDisplayPrivate::toChar
(
    COLostream& DumpStream, 
    unsigned char Character, 
    bool AsHtml, 
    int ErrorIndex, 
    int CharIndex
)
{
   bool HtmlOn = DISbinaryDisplayPrivate::doHtmlFormat(DumpStream,Character,AsHtml,ErrorIndex,CharIndex);

   if (AsHtml) DumpStream << DISlookups.HtmlCharacter[Character];
   else DumpStream << DISlookups.CharPrintable[Character];

   if (HtmlOn) DumpStream << "</font>";  
}

static const char DIShexDigits[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void DISbinaryDisplayPrivate::toHex(COLostream& DumpStream, unsigned char Character, bool AsHtml, int ErrorIndex, int CharIndex)
{
   bool HtmlOn = DISbinaryDisplayPrivate::doHtmlFormat(DumpStream,Character,AsHtml,ErrorIndex,CharIndex);
       
   char Buffer[2];
   Buffer[0] = DIShexDigits[Character >> 4];
   Buffer[1] = DIShexDigits[Character & 0xf];
   DumpStream.sink()->write(Buffer, 2);

   if (HtmlOn)
   {
      DumpStream << "</font>";  
   }
}

int DISbinaryDisplayPrivate::numberLength(int Value)
{
   int CountOfDigit=0;
   do
   {
      CountOfDigit++;
      Value = Value / 10;
   }
   while(Value > 0);
   return CountOfDigit;
}

void DISbinaryDisplay::displayData
(
   COLostream&    DumpStream, 
   const char* pBuffer, 
   int         Length, 
   int         ErrorIndex,
   int         ErrorIndexMax, // we actually ignore this currently.
   bool  AsHtml,
   int         DisplayStartByteOffset,
   int         TotalStreamSize
)
{
   int LineIndex, CharIndex;
   bool OddLine = false;
   int Padding = DISbinaryDisplayPrivate::numberLength(Length);
   if (TotalStreamSize > 0)
   {
      Padding = DISbinaryDisplayPrivate::numberLength(TotalStreamSize);
   }
   for (CharIndex = 0; CharIndex < Length; CharIndex += 16)
   {
      OddLine = !OddLine;
      
      if (AsHtml)
      {
         if (OddLine)
         {
            DumpStream << COL_T("<span class=\"ho\"><b>");
         }
         else
         {
            DumpStream << COL_T("<span class=\"he\"><b>");
         }
         
      }
      DumpStream << CharIndex+DisplayStartByteOffset;
      DumpStream << DISspace();
      DumpStream << DISspace( Padding-DISbinaryDisplayPrivate::numberLength(CharIndex+DisplayStartByteOffset) );;
      if (AsHtml)
      {
         DumpStream << "</b>";
      }      
     
      for (LineIndex = 0; 
           (LineIndex < 8) && (LineIndex + CharIndex < Length);
           LineIndex++)
      {
         DISbinaryDisplayPrivate::toHex(DumpStream, pBuffer[CharIndex + LineIndex], AsHtml, ErrorIndex, CharIndex + LineIndex);
         DumpStream << DISspace(1);
      }
      DumpStream << DISspace(2);
      while (LineIndex < 8)
      {
         DumpStream << DISspace(3);
         LineIndex++;
      }
      for (LineIndex = 8;
           (LineIndex < 16) && (LineIndex + CharIndex < Length);
           LineIndex++)
      {
         DISbinaryDisplayPrivate::toHex(DumpStream, pBuffer[CharIndex + LineIndex], AsHtml, ErrorIndex, CharIndex + LineIndex);
         DumpStream << DISspace();
      }      
      DumpStream << DISspace(2);
      while (LineIndex < 16)
      {
         DumpStream << DISspace(3);
         LineIndex++;
      }
      for (LineIndex = 0;
           (LineIndex < 8) && (LineIndex + CharIndex < Length);
           LineIndex++)
      {
         unsigned char Character = pBuffer[CharIndex + LineIndex];
         DISbinaryDisplayPrivate::toChar(DumpStream, Character, AsHtml, ErrorIndex, CharIndex+LineIndex);
      }
      DumpStream << DISspace();
      for (LineIndex = 8;
           (LineIndex < 16) && (LineIndex + CharIndex < Length);
           LineIndex++)
      {
         unsigned char Character = pBuffer[CharIndex + LineIndex];
         DISbinaryDisplayPrivate::toChar(DumpStream, Character, AsHtml, ErrorIndex, CharIndex+LineIndex);
      }
      if (AsHtml)
      {
         DumpStream << COL_T("</span>");
      }
      DumpStream << newline;

      if ((ErrorIndex != -1) && !AsHtml)
      {
         if ((ErrorIndex > CharIndex) && (ErrorIndex < CharIndex + 16))
         {
            DumpStream << ">>> ERROR IS HERE AT BYTE " << ErrorIndex-CharIndex << "<<<" << newline;   
         }
      } 
   }
}

