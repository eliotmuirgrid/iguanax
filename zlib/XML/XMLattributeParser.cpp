//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLattributeParser
//
// Description:
//
// Simple class for parsing out attributes.  It takes a string
// of the format 'name =  "life.html" id=1' and puts the attributes
// into a hash table internally.  The class is useful for parsing
// processing instructions from the expat parser which leaves the work
// up to the user to obtain the arguments.
//
// Author: Eliot Muir
// Date:   Sun 04/28/2002 
//
//---------------------------------------------------------------------------

#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLattributeParser.h>
#include <COL/COLhashmap.h>
#include <COL/COLhash.h>

class XMLattributeParserPrivate
{
public:
   XMLattributeParserPrivate()
      : pBuffer(NULL),
        AttributeTable(COLhash<COLstring>::defaultHash)
   {
   }
   ~XMLattributeParserPrivate()
   {
      free(pBuffer);
   }
   COLhashmap<COLstring, const char*> AttributeTable;

   void parse(const char* AttributeData);
   void skipWhiteSpace();
   void findEndOfAttributeName();
   char findStartOfAttributeData();
   void findEndOfAttributeData(char QuoteCharacter);

   void findCloseQuote(char QuoteCharacter);
   void findNonAlphaNumericChar();


   void throwParseError(const COLstring& Message="");

   char* pCursor;
   char* pBuffer;
   const char* pOriginal;
};

void XMLattributeParserPrivate::skipWhiteSpace()
{
   while ((*pCursor != 0)
          && ((*pCursor == 32) 
          || (*pCursor == 9)))
   {
      pCursor++;
   }
}

void XMLattributeParserPrivate::findEndOfAttributeName()
{
   while(*pCursor != 0 && (::isalpha(*pCursor) || ::isdigit(*pCursor)))
   {
      pCursor++;
   }
   if (*pCursor == 0)
   {
      throwParseError();
   }
}

void XMLattributeParserPrivate::throwParseError(const COLstring& Message)
{
   COL_ERROR_STREAM(COL_T("Error parsing attributes: ") <<
                    Message << COL_T(" in ") << pOriginal, COLerror::PreCondition);
}

void XMLattributeParserPrivate::findCloseQuote(char QuoteCharacter)
{
   while((*pCursor != QuoteCharacter) && (*pCursor != 0))
   {
      pCursor++;
   }
   if (*pCursor == 0)
   {
      throwParseError("Unbalanced quotes.");
   }
}

void XMLattributeParserPrivate::findNonAlphaNumericChar()
{
   while(::isalpha(*pCursor) || ::isdigit(*pCursor))
   {
      pCursor++;
   }
}


void XMLattributeParserPrivate::parse
(
   const char* Data 
)
{
   if (pBuffer)
   {
      free(pBuffer);
   }
   pBuffer = ::strdup(Data);
   pOriginal = Data;
   pCursor = pBuffer;

   while(*pCursor != 0)
   {
      skipWhiteSpace();
      char* pAttributeStart = pCursor;
      if (*pCursor == 0)
      {
         break;
      }
      findEndOfAttributeName();
      if (*pCursor == 0)
      {
         break;
      }
      if (*pCursor == '=')
      {
         *pCursor = 0;
         pCursor++;
         skipWhiteSpace();
      }
      else
      {
         *pCursor = 0;
         pCursor++;
         skipWhiteSpace();
         if (*pCursor != '=')
         {
            throwParseError();
         }
         pCursor++;
         skipWhiteSpace();
      }
      char* pAttributeData;
      if ((*pCursor == '\'') || (*pCursor == '"'))
      {
         // we have quoted data
         char QuoteCharacter = *pCursor;
         pCursor++;
         pAttributeData = pCursor;
         findCloseQuote(QuoteCharacter);
      }
      else
      {
         // we have unquoted data
         pAttributeData = pCursor;
         findNonAlphaNumericChar();
      }
      if (*pCursor != 0)
      {
         // i.e. we are not at the end of the attribute data
         // therefore we null terminate the attribute data and
         // go to the next byte in the string.
         *pCursor = 0;
         pCursor++;
      }      
      AttributeTable[pAttributeStart] = pAttributeData;
   }
}

XMLattributeParser::XMLattributeParser()
{
   pMember = new XMLattributeParserPrivate();
}

XMLattributeParser::~XMLattributeParser()
{
   delete pMember;
}

void XMLattributeParser::parse(const char* AttributeData)
{
   pMember->parse(AttributeData);
}

const char* XMLattributeParser::operator[](const COLstring& Name)
{
   return pMember->AttributeTable[Name];
}


