#ifndef __XML_XSD_SCHEMA_FORMATTER_H__
#define __XML_XSD_SCHEMA_FORMATTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLxsdSchemaFormatter
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

#include <COL/COLstring.h>
#include <XML/XMLschemaFormatter.h>

class XMLostream;

class XMLxsdSchemaFormatter : public XMLschemaFormatter
{
public:
   XMLxsdSchemaFormatter();
   virtual ~XMLxsdSchemaFormatter();
  
   virtual const char* description();
   virtual const char* defaultExtension();
   virtual void printOn( const XMLschema& Schema, COLostream& Stream , const XMLschemaProperties& Properties);

protected:
   virtual void printStringElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream );
   virtual void printIntegerElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream );
   virtual void printDateTimeElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream );
   virtual void printDoubleElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream );

   virtual void printEnumerationElementOn( const XMLschemaEnumeration& Type, const XMLschemaElement& Element, COLostream& Stream );
   virtual void printCollectionElementOn( const XMLschemaCollection& Type, const XMLschemaElement& Element, COLostream& Stream );
   virtual void printSequenceNodeOn( const XMLschemaSequence& SequenceNode, COLostream& Stream );
   virtual void printReferenceElementOn( const XMLschemaReference& Element, COLostream& Stream );

   virtual void printEnumerationTypeOn( const XMLschemaEnumeration& Type, COLostream& Stream );
   virtual void printCollectionTypeOn( const XMLschemaCollection& Type, COLostream& Stream );

private:
   virtual void printXsdElementOn( const char* pType, const XMLschemaElement& Element, COLostream& Stream );
   virtual void printOccursAttributesOn( const XMLschemaNode& Element );
   XMLostream* pXmlStream;
   //xsd schema values
   static const char* pXMLxsd;        // "xsd"
   static const char* pXMLschema;     // "schema"
   static const char* pXMLelement;    // "element"
   static const char* pXMLname;       // "name"
   static const char* pXMLsimple;     // "simpleType"
   static const char* pXMLrestriction;// "restriction"
   static const char* pXMLenumeration;// "enumeration"
   static const char* pXMLcomplex;    // "complexType"
   static const char* pXMLsequence;   // "sequence"
   static const char* pXMLbase;       // "base"
   static const char* pXMLtype;       // "type"
   static const char* pXMLvalue;      // "value"
   static const char* pXMLminOccurs;  // "minOccurs"
   static const char* pXMLmaxOccurs;  // "maxOccurs"
   static const char* pXMLunbounded;  // "unbounded"
   static const char* pXMLreference;  // "ref"

   //built in types
   static const char* pXMLstring;     // "xsd:string"
   static const char* pXMLinteger;    // "xsd:integer"
   static const char* pXMLdouble;     // "xsd:double"
   static const char* pXMLdateTime;     // "xsd:dateTime"

   // these are not allowed.
   XMLxsdSchemaFormatter(const XMLxsdSchemaFormatter& Original);
   XMLxsdSchemaFormatter& operator=(const XMLxsdSchemaFormatter& Original);

   //Temporary state variable - note that this can be a thread problem
   bool GlobalElement;
};
#endif // end of defensive include
