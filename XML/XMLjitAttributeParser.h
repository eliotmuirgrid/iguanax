#ifndef __XML_JIT_ATTRIBUTE_PARSER_H__
#define __XML_JIT_ATTRIBUTE_PARSER_H__
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
// This class is used in KISSBOOK
//
// Author: Eliot Muir
// Date:   Wed 05/01/2002 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
class XMLjitAttributeParserPrivate;
class COLstring;

class XMLjitAttributeParser
{
public:
   XMLjitAttributeParser(const char** pAttributeArray);
   ~XMLjitAttributeParser();

   const char* operator[](const char* pName);
private:
   XMLjitAttributeParserPrivate* pMember;
   // not allowed
   XMLjitAttributeParser(const XMLjitAttributeParser& Orig);
   XMLjitAttributeParser& operator=(const XMLjitAttributeParser& Orig);
};
#endif // end of defensive include


