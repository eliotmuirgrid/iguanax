//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc. All Rights Reserved
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
#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLschema.h>
#include <XML/XMLdtdSchemaFormatter.h>
#include <XML/XMLschemaType.h>
#include <XML/XMLschemaReference.h>
#include <XML/XMLschemaElement.h>
#include <XML/XMLschemaSequence.h>
#include <XML/XMLostream.h>

const char* XMLdtdSchemaFormatter::pXMLelement = "ELEMENT";
const char* XMLdtdSchemaFormatter::pXMLpcdata = "#PCDATA";
const char XMLdtdSchemaFormatter::XMLoptional = '?';
const char XMLdtdSchemaFormatter::XMLoptionalRepeating = '*';
const char XMLdtdSchemaFormatter::XMLrequiredRepeating = '+';

XMLdtdSchemaFormatter::XMLdtdSchemaFormatter()
{
}

XMLdtdSchemaFormatter::~XMLdtdSchemaFormatter()
{
}

const char* XMLdtdSchemaFormatter::description()
{
   static const char* pDescription = "DTD Schema";
   return pDescription;
}

const char* XMLdtdSchemaFormatter::defaultExtension()
{
   static const char* pExtension = ".dtd";
   return pExtension;
}

void XMLdtdSchemaFormatter::printOn( const XMLschema& Schema, COLostream& Stream , const XMLschemaProperties& Properties)
{
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
}

void XMLdtdSchemaFormatter::printDtdElementOn( const COLstring& Name, const COLstring& Value, COLostream& Stream )
{
   XMLostream XmlStream( Stream );
   XmlStream.outputEntityTag( pXMLelement );
   Stream << XMLostream::XMLspace << Name << XMLostream::XMLspace << '(' << Value << ')';
   XmlStream.outputTagEnd();
}

void XMLdtdSchemaFormatter::printOccursAttributesOn( const XMLschemaNode& Node, COLostream& Stream )
{
   if( Node.minOccurs() == 0 )
   {
      if( Node.maxOccurs() == 1 )
      {
         Stream << XMLoptional;
      }
      else
      {
         Stream << XMLoptionalRepeating;
      }
   }
   else if( Node.maxOccurs() > 1 )
   {
      Stream << XMLrequiredRepeating;
   }
}

void XMLdtdSchemaFormatter::printStringElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printDtdElementOn( Element.name(), pXMLpcdata, Stream );
}

void XMLdtdSchemaFormatter::printIntegerElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printDtdElementOn( Element.name(), pXMLpcdata, Stream );
}

void XMLdtdSchemaFormatter::printDateTimeElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printDtdElementOn( Element.name(), pXMLpcdata, Stream );
}

void XMLdtdSchemaFormatter::printDoubleElementOn( const XMLschemaSimple& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printDtdElementOn( Element.name(), pXMLpcdata, Stream );
}

void XMLdtdSchemaFormatter::printEnumerationElementOn( const XMLschemaEnumeration& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   printDtdElementOn( Element.name(), pXMLpcdata, Stream );
}

void XMLdtdSchemaFormatter::printCollectionElementOn( const XMLschemaCollection& Type, const XMLschemaElement& Element, COLostream& Stream )
{
   XMLostream XmlStream( Stream );
   XmlStream.outputEntityTag( pXMLelement );
   Stream << XMLostream::XMLspace << Element.name() << XMLostream::XMLspace << '(';
 
   const XMLschemaNode& MainNode = Type.mainNode();
   COLPRECONDITION(MainNode.nodeType() == XMLschemaNode::eSequence);
   const XMLschemaSequence& SequenceNode = (const XMLschemaSequence&)MainNode;
   printSequenceNodeOn(SequenceNode,Stream);
   Stream << ')';
   XmlStream.outputTagEnd();

   if (!Element.isTypeReference())
   {
      printSequenceElementsOn(SequenceNode,Stream);
   }

}  


void XMLdtdSchemaFormatter::printEnumerationTypeOn( const XMLschemaEnumeration& Type, COLostream& Stream )
{
}

void XMLdtdSchemaFormatter::printCollectionTypeOn( const XMLschemaCollection& Type, COLostream& Stream )
{
   const XMLschemaNode& MainNode = Type.mainNode();
   COLPRECONDITION(MainNode.nodeType() == XMLschemaNode::eSequence);   
   const XMLschemaSequence& SequenceNode = (const XMLschemaSequence&)MainNode;
   printSequenceElementsOn(SequenceNode,Stream);   
}

void XMLdtdSchemaFormatter::printSequenceNodeOn( const XMLschemaSequence& SequenceNode, COLostream& Stream )
{
   COLuint32 Index;
   for( Index = 0; Index < SequenceNode.countOfNodes(); Index++ )
   {
      if( Index > 0 )
      {
         Stream << ',' << XMLostream::XMLspace;
      }
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
         Stream << XMLostream::XMLspace << '(';
         printSequenceNodeOn( (const XMLschemaSequence&)Node, Stream );
         Stream << ')';
         printOccursAttributesOn( Node, Stream );
         break;
      default:
         COL_ERROR_STREAM("Unknown Nodes",0);
      }

      if (pElementToPrint)
      {
         Stream << pElementToPrint->name();
         printOccursAttributesOn( Node, Stream );
      }
   }
}

void XMLdtdSchemaFormatter::printReferenceElementOn( const XMLschemaReference& Reference, COLostream& Stream )
{
}

void XMLdtdSchemaFormatter::printSequenceElementsOn( const XMLschemaSequence& SequenceNode, COLostream& Stream )
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
