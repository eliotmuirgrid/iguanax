//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
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
#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLschema.h>
#include <XML/XMLxsdSchemaFormatter.h>
#include <XML/XMLschemaType.h>
#include <XML/XMLschemaElement.h>
#include <XML/XMLschemaSequence.h>
#include <XML/XMLschemaReference.h>
#include <XML/XMLostream.h>
#include <XML/XMLschemaProperties.h>

//xsd schema values
const char* XMLxsdSchemaFormatter::pXMLxsd = "xsd";
const char* XMLxsdSchemaFormatter::pXMLschema = "schema";
const char* XMLxsdSchemaFormatter::pXMLelement = "element";
const char* XMLxsdSchemaFormatter::pXMLname = "name";
const char* XMLxsdSchemaFormatter::pXMLsimple = "simpleType";
const char* XMLxsdSchemaFormatter::pXMLrestriction = "restriction";
const char* XMLxsdSchemaFormatter::pXMLenumeration = "enumeration";
const char* XMLxsdSchemaFormatter::pXMLcomplex = "complexType";
const char* XMLxsdSchemaFormatter::pXMLsequence = "sequence";
const char* XMLxsdSchemaFormatter::pXMLbase = "base";
const char* XMLxsdSchemaFormatter::pXMLtype = "type";
const char* XMLxsdSchemaFormatter::pXMLvalue = "value";
const char* XMLxsdSchemaFormatter::pXMLminOccurs = "minOccurs";
const char* XMLxsdSchemaFormatter::pXMLmaxOccurs = "maxOccurs";
const char* XMLxsdSchemaFormatter::pXMLunbounded = "unbounded";
const char* XMLxsdSchemaFormatter::pXMLreference = "ref";

//built in types
const char* XMLxsdSchemaFormatter::pXMLstring = "xsd:string";
const char* XMLxsdSchemaFormatter::pXMLinteger = "xsd:integer";
const char* XMLxsdSchemaFormatter::pXMLdouble = "xsd:double";
const char* XMLxsdSchemaFormatter::pXMLdateTime = "xsd:dateTime";

XMLxsdSchemaFormatter::XMLxsdSchemaFormatter():GlobalElement(false)
{
}

XMLxsdSchemaFormatter::~XMLxsdSchemaFormatter()
{
}

const char* XMLxsdSchemaFormatter::description()
{
   static const char* pDescription = "XSD Schema";
   return pDescription;
}

const char* XMLxsdSchemaFormatter::defaultExtension()
{
   static const char* pExtension = ".xsd";
   return pExtension;
}

void XMLxsdSchemaFormatter::printOn( const XMLschema& Schema, COLostream& Stream , const XMLschemaProperties& Properties)
{
   //so pointer is deleted when function exits or exception occurs
   XMLschemaPointer< XMLostream > XmlStreamSmartPointer( pXmlStream = new XMLostream( Stream ) ); 

   //<?xml version="1.0" encoding="UTF-8"?>
   pXmlStream->outputProcessingInstructionTag( XMLostream::pXMLxml );
   pXmlStream->outputAttribute( XMLostream::pXMLversion, XMLostream::pXMLversionNum );
   pXmlStream->outputAttribute( XMLostream::pXMLencoding, "UTF-8" );
   pXmlStream->outputProcessingInstructionTagEnd();

   //<xsd:schema xmlns="http://www.tibco.com/schema/ADT" targetNamespace="http://www.tibco.com/schema/ADT" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
   pXmlStream->outputTag( pXMLxsd, pXMLschema );
   if( Properties.xmlNamespace().size() )
   {
      pXmlStream->outputAttribute( XMLostream::pXMLxmlns, Properties.xmlNamespace().c_str() );
   }
   if( Properties.targetNamespace().size() )
   {
      pXmlStream->outputAttribute( "targetNamespace", Properties.targetNamespace().c_str() );
   }
   if( Properties.elementFormDefault().size() )
   {
      pXmlStream->outputAttribute( "elementFormDefault", Properties.elementFormDefault().c_str() );
   }
   pXmlStream->outputAttribute( XMLostream::pXMLxmlns, pXMLxsd, "http://www.w3.org/2001/XMLSchema" );
   pXmlStream->outputTagEnd();

   //output all the global elements
   COLuint32 Index;
   for( Index = 0; Index < Schema.countOfElements(); Index++ )
   {
      GlobalElement = true;
      printElementOn( Schema.elementAt( Index ), Stream );
   }
   GlobalElement = false;

   //output all the global types
   for( Index = 0; Index < Schema.countOfTypes(); Index++ )
   {
      printTypeOn( Schema.typeAt( Index ), Stream );
   }
   
   //</xsd:schema>
   pXmlStream->outputCloseTag();
}

void XMLxsdSchemaFormatter::printOccursAttributesOn( const XMLschemaNode& Node )
{
   if (Node.nodeType() == XMLschemaNode::eElement ||
       Node.nodeType() == XMLschemaNode::eReference)
   {
      if( !GlobalElement )
      {
         if( Node.minOccurs() != 1 ) //1 is default
         {
            pXmlStream->outputAttribute( pXMLminOccurs, Node.minOccurs() );
         }
         if( Node.maxOccurs() != 1 ) //1 is default
         {
            if( Node.maxOccurs() == XMLschemaNode::OccursUnbounded )
            {
               pXmlStream->outputAttribute( pXMLmaxOccurs, pXMLunbounded );
            }
            else
            {
               pXmlStream->outputAttribute( pXMLmaxOccurs, Node.maxOccurs() );
            }
         }
      }
   }
   else if (Node.nodeType() == XMLschemaNode::eSequence)
   {
      COLPRECONDITION(!GlobalElement);
      const XMLschemaSequence& Sequence = (const XMLschemaSequence&)Node;
      if( Sequence.minOccurs() != 1 ) //1 is default
      {
         pXmlStream->outputAttribute( pXMLminOccurs, Sequence.minOccurs() );
      }
      if( Sequence.maxOccurs() != 1 ) //1 is default
      {
         if( Sequence.maxOccurs() == XMLschemaNode::OccursUnbounded )
         {
            pXmlStream->outputAttribute( pXMLmaxOccurs, pXMLunbounded );
         }
         else
         {
            pXmlStream->outputAttribute( pXMLmaxOccurs, Sequence.maxOccurs() );
         }
      }
   }
   else
   {
      COL_ERROR_STREAM("Occurs only supported for elements and sequences",0);
   }      
}

void XMLxsdSchemaFormatter::printXsdElementOn( const char* pType, const XMLschemaElement& Element, COLostream& Stream )
{
   pXmlStream->outputTag( pXMLxsd, pXMLelement );
   pXmlStream->outputAttribute( pXMLname, Element.name().c_str() );
   pXmlStream->outputAttribute( pXMLtype, pType );
   printOccursAttributesOn( Element );
   pXmlStream->outputTagEndClose();

}

void XMLxsdSchemaFormatter::printStringElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printXsdElementOn( pXMLstring, Element, Stream );
}

void XMLxsdSchemaFormatter::printIntegerElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printXsdElementOn( pXMLinteger, Element, Stream );
}

void XMLxsdSchemaFormatter::printDateTimeElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printXsdElementOn( pXMLdateTime, Element, Stream );
}

void XMLxsdSchemaFormatter::printDoubleElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printXsdElementOn( pXMLdouble, Element, Stream );
}

void XMLxsdSchemaFormatter::printEnumerationElementOn( const XMLschemaEnumeration& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   pXmlStream->outputTag( pXMLxsd, pXMLelement );
   pXmlStream->outputAttribute( pXMLname, Element.name().c_str() );
   if( !Element.isTypeReference() )
   {
      pXmlStream->outputTagEnd();
      printEnumerationTypeOn( Type, Stream );
      pXmlStream->outputCloseTag();
   }
   else
   {
      pXmlStream->outputAttribute( pXMLtype, Type.name().c_str() );
      pXmlStream->outputTagEndClose();
   }
}

void XMLxsdSchemaFormatter::printCollectionElementOn( const XMLschemaCollection& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   pXmlStream->outputTag( pXMLxsd, pXMLelement );
   pXmlStream->outputAttribute( pXMLname, Element.name().c_str() );
  
   if( !Element.isTypeReference() )
   {
      printOccursAttributesOn( Element );
      pXmlStream->outputTagEnd();
      GlobalElement = false;
      printCollectionTypeOn( Type, Stream );
      pXmlStream->outputCloseTag();
   }
   else
   {
      printOccursAttributesOn( Element );
      pXmlStream->outputAttribute( pXMLtype, Type.name().c_str() );
      pXmlStream->outputTagEndClose();
   }
}

void XMLxsdSchemaFormatter::printEnumerationTypeOn( const XMLschemaEnumeration& Type, COLostream& Stream )
{
   pXmlStream->outputTag( pXMLxsd, pXMLsimple );
   pXmlStream->outputTagEnd();

   pXmlStream->outputTag( pXMLxsd, pXMLrestriction );
   pXmlStream->outputAttribute( pXMLbase, pXMLstring );
   pXmlStream->outputTagEnd();

   for( COLuint32 Index = 0; Index < Type.countOfValues(); Index++ )
   {
      pXmlStream->outputTag(pXMLxsd,pXMLenumeration );
      pXmlStream->outputAttribute( pXMLvalue, Type.valueAt( Index ).c_str() );
      pXmlStream->outputTagEndClose();
     
   }
   pXmlStream->outputCloseTag();
   pXmlStream->outputCloseTag();
}

void XMLxsdSchemaFormatter::printCollectionTypeOn( const XMLschemaCollection& Type, COLostream& Stream )
{
   pXmlStream->outputTag( pXMLxsd, pXMLcomplex );
   if( Type.name().size() )
   {
      pXmlStream->outputAttribute( pXMLname, Type.name().c_str() );
   }
   pXmlStream->outputTagEnd();

   const XMLschemaNode& MainNode = Type.mainNode();
   switch(MainNode.nodeType())
   {
      case XMLschemaNode::eSequence:
         printSequenceNodeOn((XMLschemaSequence&)MainNode,Stream);
         break;
      default:
         COL_ERROR_STREAM("Only Sequence Main Nodes supported",0);
   }

   pXmlStream->outputCloseTag();
}

void XMLxsdSchemaFormatter::printSequenceNodeOn( const XMLschemaSequence& SequenceNode, COLostream& Stream )
{
   pXmlStream->outputTag( pXMLxsd, pXMLsequence );
   printOccursAttributesOn( (XMLschemaNode&)SequenceNode );   
   pXmlStream->outputTagEnd();

   for( COLuint32 Index = 0; Index < SequenceNode.countOfNodes(); Index++ )
   {
      const XMLschemaNode& Node = SequenceNode.nodeAt(Index);
      switch(Node.nodeType())
      {
      case XMLschemaNode::eReference:
         printReferenceElementOn((XMLschemaReference&)Node,Stream);
         break;
      case XMLschemaNode::eElement:
         printElementOn((XMLschemaElement&)Node,Stream);
         break;
      case XMLschemaNode::eSequence:
         printSequenceNodeOn((XMLschemaSequence&)Node,Stream);
         break;
      default:
         COL_ERROR_STREAM("Unknown node type",0);
      }
   }
   pXmlStream->outputCloseTag();
}

void XMLxsdSchemaFormatter::printReferenceElementOn( const XMLschemaReference& Reference, COLostream& Stream )
{
   //we expect only elements to be referenceable
   pXmlStream->outputTag( pXMLxsd, pXMLelement );
   pXmlStream->outputAttribute( pXMLreference, Reference.referencedElement().name().c_str() );
   printOccursAttributesOn( Reference );
   pXmlStream->outputTagEndClose();
}
