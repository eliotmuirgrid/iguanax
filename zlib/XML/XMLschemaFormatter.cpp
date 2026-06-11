#include "XMLprecomp.h"
#pragma hdrstop
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


#include <XML/XMLschemaFormatter.h>
#include <XML/XMLschemaElement.h>
#include <XML/XMLschemaSequence.h>
#include <XML/XMLschemaType.h>
#include <XML/XMLbiztalkSchemaFormatter.h>
#include <XML/XMLxsdSchemaFormatter.h>
#include <XML/XMLdtdSchemaFormatter.h>
#include <LEG/LEGrefHashTable.h>

void XMLschemaFormatter::printElementOn( const XMLschemaElement& Element, COLostream& Stream )
{
   switch( Element.type().id() )
   {
      case XMLschemaType::eSimple:
         {
            const XMLschemaSimple* pSimple = dynamic_cast<const XMLschemaSimple*>(&Element.type());
            COLPRECONDITION(pSimple)
            switch(pSimple->simpleId())
            {
               case XMLschemaSimple::eString:
                  printStringElementOn( *pSimple, Element, Stream );
                  break;
               case XMLschemaSimple::eInteger:
                  printIntegerElementOn( *pSimple, Element, Stream );
                  break;
               case XMLschemaSimple::eDateTime:
                  printDateTimeElementOn( *pSimple, Element, Stream );
                  break;
               case XMLschemaSimple::eDouble:
                  printDoubleElementOn( *pSimple, Element, Stream );
                  break;
               default:
                  break;
            }
         }
         break;
      case XMLschemaType::eEnumeration:
         printEnumerationElementOn( (XMLschemaEnumeration&)Element.type(), Element, Stream );
         break;
      case XMLschemaType::eCollection:
         printCollectionElementOn( (XMLschemaCollection&)Element.type(), Element, Stream );
         break;
      default:
         break;
   }
}

void XMLschemaFormatter::printTypeOn( const XMLschemaType& Type, COLostream& Stream )
{
   switch( Type.id() )
   {
      case XMLschemaType::eEnumeration:
         printEnumerationTypeOn( (XMLschemaEnumeration&)Type, Stream );
         break;
      case XMLschemaType::eCollection:
         printCollectionTypeOn( (XMLschemaCollection&)Type, Stream );
         break;
      default:
         break;
   }
}

struct XMLschemaFormatterFactory : public LEGrefHashTable< COLuint32, XMLschemaFormatter* >
{
   XMLschemaFormatterFactory()
   {
      XMLschemaFormatter* pFormatter = 0;

      pFormatter = new XMLdtdSchemaFormatter();
      operator[]( XMLschemaFormatter::eDtd ) = pFormatter;

      pFormatter = new XMLxsdSchemaFormatter();
      operator[]( XMLschemaFormatter::eXsd ) = pFormatter;

      pFormatter = new XMLbiztalkSchemaFormatter();
      operator[]( XMLschemaFormatter::eBiztalk ) = pFormatter;
   }

   virtual ~XMLschemaFormatterFactory()
   {
      try
      {
         Iterator Iterator( *this );
         COLuint32 Id;
         XMLschemaFormatter* pFormatter;
         while( Iterator.iterateNext( Id, pFormatter ) )
         {
            delete pFormatter;
         }
      }
      catch(const COLerror& Error)
      {
         XML_LOG(Error);
      }
   }
};

static XMLschemaFormatterFactory& SchemaFormatterFactory()
{
   static XMLschemaFormatterFactory Instance;
   return Instance;
}

XMLschemaFormatter& XMLschemaFormatter::getFormatter( XMLschemaFormatter::EFormatters Id )
{
   if( !SchemaFormatterFactory().has( Id ) )
   {
      COL_ERROR_STREAM(COL_T("Unknown schema format ") << Id << COL_T(" requested from schema formatter factory."), 
                       COLerror::PreCondition);
   }
   XMLschemaFormatter* pFormatter =  SchemaFormatterFactory()[ Id ];
   COLPRECONDITION( pFormatter != NULL );
   return *pFormatter;
}

size_t XMLschemaFormatter::countOfFormatter()
{
   return SchemaFormatterFactory().size();
}

XMLschemaFormatter& XMLschemaFormatter::getFormatter( size_t Index )
{
   COLuint32 Key;
   XMLschemaFormatter* pFormatter = 0;
   XMLschemaFormatterFactory::Iterator Iterator( SchemaFormatterFactory() );
   COLuint32 FormatIndex = 0;
   do
   {
      if( !Iterator.iterateNext( Key, pFormatter ) )
      {
         COL_ERROR_STREAM("Unknown schema format requested from schema formatter factory.", COLerror::PreCondition);
      }
   } while( FormatIndex++ != Index );
   return *pFormatter;
}
