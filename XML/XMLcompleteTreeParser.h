#ifndef __XML_COMPLETE_TREE_PARSER_H_
#define __XML_COMPLETE_TREE_PARSER_H_
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLcompleteTreeParser
//
// Description:
//
// The class does a complete parse of an XML document 
// Incomplete and obsolete piece of code.
//
// Author: Eliot Muir
// Date:   Sun 06/03/2001 
//
//---------------------------------------------------------------------------

#include <XML/XMLexpatParser.h>
class COLstring;
class XMLcompleteTreeParserPrivate;
class XMLparsedTag;

class XMLcompleteTreeParser : public XMLexpatParser
{
public:
   XMLcompleteTreeParser();
   virtual ~XMLcompleteTreeParser();

   void parse(const COLstring& XMLdata, XMLparsedTag* pResultTree);

   void onStartElement(const char* Name);
   void onEndElement(const char* Name);
   void onCharacterData(const char* Data, int Length);
private:
   XMLcompleteTreeParserPrivate* pMember;
   // not allowed
   XMLcompleteTreeParser(const XMLcompleteTreeParser& Orig);
   XMLcompleteTreeParser& operator=(const XMLcompleteTreeParser& Orig);
};
#endif // end of defensive include


