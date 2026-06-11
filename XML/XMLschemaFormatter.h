#ifndef __XML_SCHEMA_FORMATTER_H__
#define __XML_SCHEMA_FORMATTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLschemaFormatter
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

class XMLschema;
class XMLschemaElement;
class XMLschemaType;
class XMLschemaSimple;
class XMLschemaEnumeration;
class XMLschemaCollection;
class COLostream;
class XMLschemaSequence;
class XMLschemaNode;
class XMLschemaProperties;
class XMLschemaReference;

class XMLschemaFormatter
{
public:
   XMLschemaFormatter(){}
   virtual ~XMLschemaFormatter(){}
  
   virtual const char* description() = 0;
   virtual const char* defaultExtension() = 0;
   virtual void printOn( const XMLschema& Schema, COLostream& Stream , const XMLschemaProperties& Properties) = 0;
   void printElementOn( const XMLschemaElement& Element, COLostream& Stream );
   void printTypeOn( const XMLschemaType& Type, COLostream& Stream );

   //factory methods
   enum EFormatters
   {
      eXsd = 0,
      eDtd,
      eBiztalk
   };
   static XMLschemaFormatter& getFormatter( EFormatters Id );
   static size_t countOfFormatter();
   static XMLschemaFormatter& getFormatter( size_t Index );

protected:
   virtual void printReferenceElementOn( const XMLschemaReference& Element, COLostream& Stream ) = 0;

   virtual void printStringElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream ) = 0;
   virtual void printIntegerElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream ) = 0;
   virtual void printDoubleElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream ) = 0;
   virtual void printDateTimeElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream ) = 0;
   
   virtual void printEnumerationElementOn( const XMLschemaEnumeration& Type, const XMLschemaElement& Element, COLostream& Stream ) = 0;
   virtual void printCollectionElementOn( const XMLschemaCollection& Type, const XMLschemaElement& Element, COLostream& Stream ) = 0;
   virtual void printSequenceNodeOn( const XMLschemaSequence& SequenceNode, COLostream& Stream ) = 0;

   virtual void printEnumerationTypeOn( const XMLschemaEnumeration& Type, COLostream& Stream ) = 0;
   virtual void printCollectionTypeOn( const XMLschemaCollection& Type, COLostream& Stream ) = 0;
private:
   XMLschemaFormatter(const XMLschemaFormatter& Original);
   XMLschemaFormatter& operator=(const XMLschemaFormatter& Original);
};
#endif // end of defensive include
