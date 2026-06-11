//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: XMLcompleteTreeParser
//
// Description:
//
// Parses XML into a tree
//
// Author: Eliot Muir
// Date:   Sun 06/03/2001 
//
//---------------------------------------------------------------------------
#include "XMLprecomp.h"
#pragma hdrstop

#include <string.h>
#include <EXP/expat.h>
#include <XML/XMLcompleteTreeParser.h>
#include <XML/XMLparsedTag.h>

class XMLcompleteTreeParserPrivate
{
public:
   XMLcompleteTreeParserPrivate() : pCurrentTag(NULL) {}
   ~XMLcompleteTreeParserPrivate() {}

   XMLparsedTag* pCurrentTag;
};

XMLcompleteTreeParser::XMLcompleteTreeParser()
{
   pMember = new XMLcompleteTreeParserPrivate();
}

XMLcompleteTreeParser::~XMLcompleteTreeParser()
{
   delete pMember;
}

void XMLcompleteTreeParser::parse
(
   const COLstring& XMLdata,
   XMLparsedTag*    pResultTree
)
{
   COLPRECONDITION(pResultTree != NULL);
   pResultTree->clear();
   pMember->pCurrentTag = pResultTree;
   XMLexpatParser::parse(XMLdata);
}

void XMLcompleteTreeParser::onStartElement(const char* Name)
{
   COLPRECONDITION(pMember->pCurrentTag != NULL);
   XMLparsedTag* pNewTag = new XMLparsedTag(Name);
   pMember->pCurrentTag->addTag(pNewTag);
   pMember->pCurrentTag = pNewTag;
}

void XMLcompleteTreeParser::onEndElement(const char* Name)
{
   pMember->pCurrentTag = pMember->pCurrentTag->parent();
   COLPRECONDITION(pMember->pCurrentTag != NULL);
}

void XMLcompleteTreeParser::onCharacterData(const char* Data, int Length)
{
   COLPRECONDITION(pMember->pCurrentTag != NULL);
   COLstring Value(Data, Length);
   if (!Value.isWhitespace())
   {
      pMember->pCurrentTag->setData(Value);
   }
}
