#ifndef __XML_DTD_SCHEMA_FORMATTER_H__
#define __XML_DTD_SCHEMA_FORMATTER_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2007 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: XMLdtdSchemaFormatter
//
// Description:
//
// This class is used to generate a  Schema description
// of the table grammar.
//
// Author: Ryan Dewsbury
// Date:   Mon 05/28/2001 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <XML/XMLschemaFormatter.h>

class COLstring;
class XMLostream;

class XMLdtdSchemaFormatter : public XMLschemaFormatter
{
public:
   XMLdtdSchemaFormatter();
   virtual ~XMLdtdSchemaFormatter();
  
   virtual const char* description();
   virtual const char* defaultExtension();
   virtual void printOn( const XMLschema& Schema, COLostream& Stream , const XMLschemaProperties& Properties);

protected:
   virtual void printReferenceElementOn( const XMLschemaReference& Element, COLostream& Stream );
   virtual void printStringElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream );
   virtual void printIntegerElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream );
   virtual void printDateTimeElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream );
   virtual void printDoubleElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream );
   virtual void printEnumerationElementOn( const XMLschemaEnumeration& Type, const XMLschemaElement& Element, COLostream& Stream );
   virtual void printCollectionElementOn( const XMLschemaCollection& Type, const XMLschemaElement& Element, COLostream& Stream );
   virtual void printSequenceNodeOn( const XMLschemaSequence& SequenceNode, COLostream& Stream );

   virtual void printEnumerationTypeOn( const XMLschemaEnumeration& Type, COLostream& Stream );
   virtual void printCollectionTypeOn( const XMLschemaCollection& Type, COLostream& Stream );


private:
   static const char* pXMLelement; // "ELEMENT"
   static const char* pXMLpcdata;  // "#PCDATA"
   static const char XMLoptional;  // '?'
   static const char XMLoptionalRepeating; // '*'
   static const char XMLrequiredRepeating; // '+'

   void printSequenceElementsOn( const XMLschemaSequence& SequenceNode, COLostream& Stream );
   void printDtdElementOn( const COLstring& Name, const COLstring& Value, COLostream& Stream );
   void printOccursAttributesOn( const XMLschemaNode& Node, COLostream& Stream );
   
   XMLdtdSchemaFormatter(const XMLdtdSchemaFormatter& Original);
   XMLdtdSchemaFormatter& operator=(const XMLdtdSchemaFormatter& Original);
};
#endif // end of defensive include
