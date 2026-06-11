#ifndef __XML_OSTREAM__
#define __XML_OSTREAM__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLostream
//
// Description:
//
// This class makes it convenient and clean to implement
// outputting all the XML tags etc.  It's used by XMLbiztalkSchemaFormatter
// and XMLxsdSchemaFormatter to make implementation of these classes easier.
//
// Author: Ryan Dewsbury
// Date:   Mon 05/28/2001 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLostream.h>

struct XMLostreamPrivate;
class XMLostream
{
public:
   XMLostream( COLostream& Stream_ );
   ~XMLostream();

   void outputEntityTag( const char* pName );

   void outputProcessingInstructionTag( const char* pName );
   void outputProcessingInstructionTagEnd();

   void outputTag( const char* pName );
   void outputTag( const char* pNamespace, const char* pName );
   void outputTagEnd();
   void outputTagEndClose();

   void outputCloseTag();

   void outputAttribute( const char* pName, const char* pValue );
   void outputAttribute( const char* pNamespace, const char* pName, const char* pValue );
   void outputAttribute( const char* pName, COLuint32 Value );
   void outputAttribute( const char* pNamespace, const char* pName, COLuint32 Value );

   static const char* pXMLxml; // "xml"
   static const char* pXMLxmlns; // "xmlns"
   static const char* pXMLversion; // "version"
   static const char* pXMLversionNum; // "1.0"
   static const char* pXMLencoding; // "encoding"

   //special XML characters
   static const char XMLstartTag;
   static const char XMLendTag;
   static const char XMLprocessingInstruction;
   static const char XMLspace;
   static const char XMLequal;
   static const char XMLquote;
   static const char XMLnamespace;
   static const char XMLtab;
   static const char XMLend;
   static const char XMLexclamation;
private:
   XMLostream(const XMLostream& Orig);  // private not allowed
   XMLostream& operator=(const XMLostream& Orig); // private not allowed

   XMLostreamPrivate* pMember;
};


#endif // end of defensive include
