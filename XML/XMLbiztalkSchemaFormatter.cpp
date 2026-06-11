//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc. All Rights Reserved
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
#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLschema.h>
#include <XML/XMLschemaReference.h>
#include <XML/XMLbiztalkSchemaFormatter.h>
#include <XML/XMLschemaType.h>
#include <XML/XMLschemaElement.h>
#include <XML/XMLschemaSequence.h>
#include <XML/XMLostream.h>

XMLbiztalkSchemaFormatter::XMLbiztalkSchemaFormatter()
{
}

XMLbiztalkSchemaFormatter::~XMLbiztalkSchemaFormatter()
{
}

const char* XMLbiztalkSchemaFormatter::description()
{
   static const char* pDescription = "Microsoft Biztalk XML Schema";
   return pDescription;
}

const char* XMLbiztalkSchemaFormatter::defaultExtension()
{
   static const char* pExtension = ".xml";
   return pExtension;
}

//elements
const char* XMLbiztalkSchemaFormatter::pXMLschema = "Schema";
const char* XMLbiztalkSchemaFormatter::pXMLselectionFields = "SelectionFields";
const char* XMLbiztalkSchemaFormatter::pXMLelement = "element";
const char* XMLbiztalkSchemaFormatter::pXMLgroup = "group";
const char* XMLbiztalkSchemaFormatter::pXMLelementType = "ElementType";
const char* XMLbiztalkSchemaFormatter::pXMLfieldInfo = "FieldInfo";
const char* XMLbiztalkSchemaFormatter::pXMLrecordInfo = "RecordInfo";

//attributes
const char* XMLbiztalkSchemaFormatter::pXMLname = "name";
const char* XMLbiztalkSchemaFormatter::pXMLtype = "type";
const char* XMLbiztalkSchemaFormatter::pXMLBizTalkServerEditorTool_Version = "BizTalkServerEditorTool_Version";
const char* XMLbiztalkSchemaFormatter::pXMLroot_reference = "root_reference";
const char* XMLbiztalkSchemaFormatter::pXMLstandard = "standard";
const char* XMLbiztalkSchemaFormatter::pXMLcontent = "content";
const char* XMLbiztalkSchemaFormatter::pXMLmodel = "model";
const char* XMLbiztalkSchemaFormatter::pXMLorder = "order";
const char* XMLbiztalkSchemaFormatter::pXMLminOccurs = "minOccurs";
const char* XMLbiztalkSchemaFormatter::pXMLmaxOccurs = "maxOccurs";

//namespaces
const char* XMLbiztalkSchemaFormatter::pXMLb = "b";
const char* XMLbiztalkSchemaFormatter::pXMLd = "d";

//types
const char* XMLbiztalkSchemaFormatter::pXMLstring = "string";
const char* XMLbiztalkSchemaFormatter::pXMLint = "int";
const char* XMLbiztalkSchemaFormatter::pXMLdateTime = "dateTime";
const char* XMLbiztalkSchemaFormatter::pXMLdouble = "double";

//values
const char* XMLbiztalkSchemaFormatter::pXMLtextOnly = "textOnly";
const char* XMLbiztalkSchemaFormatter::pXMLeltOnly = "eltOnly";
const char* XMLbiztalkSchemaFormatter::pXMLclosed = "closed";
const char* XMLbiztalkSchemaFormatter::pXMLseq = "seq";
const char* XMLbiztalkSchemaFormatter::pXMLunbounded = "*";

void XMLbiztalkSchemaFormatter::printOn( const XMLschema& Schema, COLostream& Stream, const XMLschemaProperties& Properties )
{
   //so pointer is deleted when function exits or exception occurs
   XMLschemaPointer< XMLostream > XmlStreamSmartPointer( pXmlStream = new XMLostream( Stream ) ); 

   const char* pRoot = Schema.elementAt( 0 ).name().c_str();
   XMLostream XmlStream( Stream );

   //<?xml version="1.0" encoding="UTF-8"?>
   pXmlStream->outputProcessingInstructionTag( XMLostream::pXMLxml );
   pXmlStream->outputAttribute( XMLostream::pXMLversion, XMLostream::pXMLversionNum );
   pXmlStream->outputAttribute( XMLostream::pXMLencoding, "UTF-8" );
   pXmlStream->outputProcessingInstructionTagEnd();

   /* <Schema name="Chameleon_Import"
        b:BizTalkServerEditorTool_Version="1.0"
        b:root_reference="Chameleon_Import"
        b:standard="XML"
        xmlns="urn:schemas-microsoft-com:xml-data"
        xmlns:b="urn:schemas-microsoft-com:BizTalkServer"
        xmlns:d="urn:schemas-microsoft-com:datatypes">
   */
   pXmlStream->outputTag( pXMLschema );
   pXmlStream->outputAttribute( pXMLname, pRoot );
   pXmlStream->outputAttribute( pXMLb, pXMLBizTalkServerEditorTool_Version, "1.0" );
   pXmlStream->outputAttribute( pXMLb, pXMLroot_reference, pRoot );
   pXmlStream->outputAttribute( pXMLb, pXMLstandard, "XML" );
   pXmlStream->outputAttribute( XMLostream::pXMLxmlns, "urn:schemas-microsoft-com:xml-data" );
   pXmlStream->outputAttribute( XMLostream::pXMLxmlns, pXMLb, "urn:schemas-microsoft-com:BizTalkServer" );
   pXmlStream->outputAttribute( XMLostream::pXMLxmlns, pXMLd, "urn:schemas-microsoft-com:datatypes" );
   pXmlStream->outputTagEnd();

   // <b:SelectionFields/>
   pXmlStream->outputTag( pXMLb, pXMLselectionFields );
   pXmlStream->outputTagEndClose();

   //output all the global elements
   COLuint32 Index;
   for( Index = 0; Index < Schema.countOfElements(); Index++ )
   {
      printElementOn( Schema.elementAt( Index ), Stream );
   }

   //output all the global types
   for( Index = 0; Index < Schema.countOfTypes(); Index++ )
   {
      printTypeOn( Schema.typeAt( Index ), Stream );
   }
   
   //</Schema>
   pXmlStream->outputCloseTag();
}

void XMLbiztalkSchemaFormatter::printOccursAttributesOn( const XMLschemaNode& Node )
{
   pXmlStream->outputAttribute( pXMLminOccurs, Node.minOccurs() );
   if( Node.maxOccurs() == XMLschemaNode::OccursUnbounded )
   {
      pXmlStream->outputAttribute( pXMLmaxOccurs, pXMLunbounded );
   }
   else
   {
      pXmlStream->outputAttribute( pXMLmaxOccurs, Node.maxOccurs() );
   }
}

void XMLbiztalkSchemaFormatter::printBiztalkElementOn( const char* pType, const XMLschemaElement& Element, COLostream& Stream )
{
   pXmlStream->outputTag( pXMLelementType );
   pXmlStream->outputAttribute( pXMLname, Element.name().c_str() );
   pXmlStream->outputAttribute( pXMLmodel, pXMLclosed );
   pXmlStream->outputAttribute( pXMLcontent, pXMLtextOnly );
   pXmlStream->outputAttribute( pXMLd, pXMLtype, pType );
   pXmlStream->outputTagEnd();

   // <b:FieldInfo/>
   pXmlStream->outputTag( pXMLb, pXMLfieldInfo );
   pXmlStream->outputTagEndClose();

   pXmlStream->outputCloseTag();
}

void XMLbiztalkSchemaFormatter::printStringElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printBiztalkElementOn( pXMLstring, Element, Stream );   
}

void XMLbiztalkSchemaFormatter::printIntegerElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printBiztalkElementOn( pXMLint, Element, Stream );   
}

void XMLbiztalkSchemaFormatter::printDateTimeElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printBiztalkElementOn( pXMLdateTime, Element, Stream );   
}

void XMLbiztalkSchemaFormatter::printDoubleElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printBiztalkElementOn( pXMLdouble, Element, Stream );   
}

void XMLbiztalkSchemaFormatter::printEnumerationElementOn( const XMLschemaEnumeration& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printBiztalkElementOn( pXMLstring, Element, Stream );   
}

void XMLbiztalkSchemaFormatter::printCollectionElementOn( const XMLschemaCollection& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   const XMLschemaNode& MainNode = Type.mainNode();
   COLPRECONDITION(MainNode.nodeType() == XMLschemaNode::eSequence);
   const XMLschemaSequence& SequenceNode = (const XMLschemaSequence&)MainNode;

   if( !Element.isTypeReference() )
   {
      printSequenceElementsOn(SequenceNode,Stream);
   }

   XMLostream XmlStream( Stream );
   pXmlStream->outputTag( pXMLelementType );
   pXmlStream->outputAttribute( pXMLname, Element.name().c_str() );
   pXmlStream->outputAttribute( pXMLmodel, pXMLclosed );
   pXmlStream->outputAttribute( pXMLcontent, pXMLeltOnly );
   pXmlStream->outputTagEnd();
   
   // <b:RecordInfo/>
   pXmlStream->outputTag( pXMLb, pXMLrecordInfo );
   pXmlStream->outputTagEndClose();

   printSequenceNodeOn((XMLschemaSequence&)MainNode,Stream);

   pXmlStream->outputCloseTag();
}

void XMLbiztalkSchemaFormatter::printEnumerationTypeOn( const XMLschemaEnumeration& Type, COLostream& Stream )
{
   //nothing to do
}

void XMLbiztalkSchemaFormatter::printCollectionTypeOn( const XMLschemaCollection& Type, COLostream& Stream )
{
   const XMLschemaNode& MainNode = Type.mainNode();
   COLPRECONDITION(MainNode.nodeType() == XMLschemaNode::eSequence);   
   const XMLschemaSequence& SequenceNode = (const XMLschemaSequence&)MainNode;
   printSequenceElementsOn(SequenceNode,Stream);
}

const char* XMLbiztalkSchemaFormatter::schemaTypeToString(const XMLschemaType& Type) const
{
   const char* pReturn = pXMLstring;
   const XMLschemaSimple* pSimple = dynamic_cast<const XMLschemaSimple*>(&Type);
   if (pSimple)
   {
      switch(pSimple->simpleId())
      {
      case XMLschemaSimple::eInteger:
         pReturn = pXMLint;
         break;
      case XMLschemaSimple::eDouble:
         pReturn = pXMLdouble;
         break;
      case XMLschemaSimple::eDateTime:
         pReturn = pXMLdateTime;
         break;
      case XMLschemaSimple::eString:
      default:
         break;
      }
   }
   return pReturn;   
}

void XMLbiztalkSchemaFormatter::printReferenceElementOn( const XMLschemaReference& Reference, COLostream& Stream )
{
   //do nothing
}

void XMLbiztalkSchemaFormatter::printSequenceNodeOn( const XMLschemaSequence& SequenceNode, COLostream& Stream )
{
   pXmlStream->outputTag( pXMLgroup );  
   pXmlStream->outputAttribute( pXMLorder, pXMLseq );
   printOccursAttributesOn( SequenceNode );
   pXmlStream->outputTagEnd();

   for( COLuint32 Index = 0; Index < SequenceNode.countOfNodes(); Index++ )
   {
      const XMLschemaNode& Node = SequenceNode.nodeAt(Index);
      const XMLschemaElement* pElementToPrint = 0;
      switch(Node.nodeType())
      {
      case XMLschemaNode::eElement:
         { 
            pElementToPrint = dynamic_cast<const XMLschemaElement*>(&Node);
            COLPRECONDITION(pElementToPrint);
         }
         break;         
      case XMLschemaNode::eReference:
         {
            const XMLschemaReference* pReference = dynamic_cast<const XMLschemaReference*>(&Node);
            COLPRECONDITION(pReference)
            pElementToPrint = &pReference->referencedElement();
            COLPRECONDITION(pElementToPrint);
         }
         break;
      case XMLschemaNode::eSequence:
         pElementToPrint = 0;
         printSequenceNodeOn((const XMLschemaSequence&)Node,Stream);
         break;
      default:
         COL_ERROR_STREAM("Unknown node type",0);
      }

      if (pElementToPrint)
      {
         pXmlStream->outputTag( pXMLelement );
         pXmlStream->outputAttribute( pXMLtype, pElementToPrint->name().c_str() );
         printOccursAttributesOn( Node );
         pXmlStream->outputTagEndClose();
      }
   }
   pXmlStream->outputCloseTag();
}

void XMLbiztalkSchemaFormatter::printSequenceElementsOn( const XMLschemaSequence& SequenceNode, COLostream& Stream )
{
   for( COLuint32 Index = 0; Index < SequenceNode.countOfNodes(); Index++ )
   {
      const XMLschemaNode& Node = SequenceNode.nodeAt(Index);
      switch(Node.nodeType())
      {
      case XMLschemaNode::eReference:
         printReferenceElementOn((const XMLschemaReference&)Node,Stream);
         break;
      case XMLschemaNode::eElement:
         printElementOn((const XMLschemaElement&)Node,Stream);
         break;
      case XMLschemaNode::eSequence:
         printSequenceElementsOn((const XMLschemaSequence&)Node,Stream);
         break;
      default:
         COL_ERROR_STREAM("Unknown Node",0)
         break; 
      }
   }
}
