//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLjitAttributeParser
//
// Description:
//
// This class is a "Just in time" attribute parser designed to
// work with the expat parser.  The idea is that it is a waste of
// processing time to always put the array of attribute arguments
// into a hash table for future lookup.
//
// So we used this class to make it convenient to find named arguments
// from the name array.  We can encapsulate various techniques for getting
// the attribute data out, ranging from brute force search to using a hash
// table etc. to optimize parsing speed
//
// Author: Eliot Muir
// Date:   Wed 05/01/2002 
//
//---------------------------------------------------------------------------
#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLjitAttributeParser.h>

class XMLjitAttributeParserPrivate
{
public:
   XMLjitAttributeParserPrivate(const char** atts) 
      : pAttributeArray(NULL)
   {
      pAttributeArray = atts;
   }

   ~XMLjitAttributeParserPrivate()
   {
      pAttributeArray = NULL;
   }
   const char** pAttributeArray;
};

XMLjitAttributeParser::XMLjitAttributeParser(const char** pAttributeArray)
{
   pMember = new XMLjitAttributeParserPrivate(pAttributeArray);
}

XMLjitAttributeParser::~XMLjitAttributeParser()
{
   delete pMember;
}

const char* XMLjitAttributeParser::operator[](const char* pName)
{
   const char** pAtt= pMember->pAttributeArray;

   while ((*pAtt != 0) && (strcmp(pName, *pAtt) != 0))
   {
      pAtt +=2;
   }
   return (*pAtt != 0) ? *(pAtt+1) : "";
}
