#ifndef __XML_BIZTALK_SCHEMA_FORMATTER_H__
#define __XML_BIZTALK_SCHEMA_FORMATTER_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2007 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: XMLbiztalkSchemaFormatter
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

class XMLostream;
class XMLbiztalkSchemaFormatter : public XMLschemaFormatter
{
public:
   XMLbiztalkSchemaFormatter();
   virtual ~XMLbiztalkSchemaFormatter();
  
   virtual const char* description();
   virtual const char* defaultExtension();
   virtual void printOn( const XMLschema& Schema, COLostream& Stream, const XMLschemaProperties& Properties );

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
   void printSequenceElementsOn( const XMLschemaSequence& SequenceNode, COLostream& Stream );
   void printOccursAttributesOn( const XMLschemaNode& Node );
   void printBiztalkElementOn( const char* pType, const XMLschemaElement& Element, COLostream& Stream );
   XMLostream* pXmlStream;

//elements
   static const char* pXMLschema; // "Schema";
   static const char* pXMLselectionFields; // "SelectionFields";
   static const char* pXMLelement; // "element";
   static const char* pXMLgroup; // "group"
   static const char* pXMLelementType; // "ElementType";
   static const char* pXMLfieldInfo; // "FieldInfo";
   static const char* pXMLrecordInfo; // "RecordInfo";

//attributes
   static const char* pXMLname; // "name";
   static const char* pXMLtype; // "type";
   static const char* pXMLBizTalkServerEditorTool_Version; // "BizTalkServerEditorTool_Version";
   static const char* pXMLroot_reference; // "root_reference";
   static const char* pXMLstandard; // "standard";
   static const char* pXMLcontent; // "content";
   static const char* pXMLmodel; // "model";
   static const char* pXMLorder; // "order";
   static const char* pXMLminOccurs; // "minOccurs";
   static const char* pXMLmaxOccurs; // "maxOccurs";

//namespaces
   static const char* pXMLb; // "b";
   static const char* pXMLd; // "d";

//types
   static const char* pXMLstring; // "string";
   static const char* pXMLint; // "int";
   static const char* pXMLdateTime; // "dateTime";
   static const char* pXMLdouble; // "double";

//values
   static const char* pXMLtextOnly; // "textOnly";
   static const char* pXMLeltOnly; // "eltOnly";
   static const char* pXMLclosed; // "closed";
   static const char* pXMLseq; // "seq";
   static const char* pXMLunbounded; // "*";

   const char* schemaTypeToString(const XMLschemaType& Type) const;

   XMLbiztalkSchemaFormatter(const XMLbiztalkSchemaFormatter& Original);
   XMLbiztalkSchemaFormatter& operator=(const XMLbiztalkSchemaFormatter& Original);
};
#endif // end of defensive include
