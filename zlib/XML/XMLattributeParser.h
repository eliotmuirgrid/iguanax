 #ifndef __XML_ATTRIBUTE_PARSER_H__
#define __XML_ATTRIBUTE_PARSER_H__
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

#include <COL/COLminimumInclude.h>
class XMLattributeParserPrivate;
class COLstring;

class XMLattributeParser
{
public:
   XMLattributeParser();
   ~XMLattributeParser();

   void parse(const char* AttributeData);

   const char* operator[](const COLstring& Name);
   
private:
   XMLattributeParserPrivate* pMember;
   // not allowed
   XMLattributeParser(const XMLattributeParser& Orig);
   XMLattributeParser& operator=(const XMLattributeParser& Orig);
};
#endif // end of defensive include


