//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLschemaType
//
// Description:
//
// This class represents an type within the schema.  See the
// intranet for more information on what this is about.
//
// Author: Ryan Dewsbury
// Date:   Mon 05/28/2001 
//
//---------------------------------------------------------------------------
#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLschemaType.h>
#include <XML/XMLschemaElement.h>
#include <XML/XMLschemaReference.h>
#include <XML/XMLschemaSequence.h>
#include <XML/XMLschema.h>
#include <LEG/LEGvector.h>

XMLschemaSimple XMLschemaSimple::String( XMLschemaSimple::eString );
XMLschemaSimple XMLschemaSimple::Integer( XMLschemaSimple::eInteger );
XMLschemaSimple XMLschemaSimple::Double( XMLschemaSimple::eDouble );
XMLschemaSimple XMLschemaSimple::DateTime( XMLschemaSimple::eDateTime );

struct XMLschemaEnumerationPrivate
{
   LEGvector< COLstring > Enumeration;
};

XMLschemaEnumeration::XMLschemaEnumeration( const COLstring& Name ):
   XMLschemaType( Name )
{
   pMember = new XMLschemaEnumerationPrivate();
}

XMLschemaEnumeration::XMLschemaEnumeration(): XMLschemaType( "" )
{
   pMember = new XMLschemaEnumerationPrivate();
}

XMLschemaEnumeration::~XMLschemaEnumeration()
{
   delete pMember;
}

COLstring& XMLschemaEnumeration::valueAt(COLuint32 EnumIndex)
{
   COLPRECONDITION( EnumIndex < pMember->Enumeration.size() );
   return pMember->Enumeration[ EnumIndex ];
}

const COLstring& XMLschemaEnumeration::valueAt(COLuint32 EnumIndex) const
{
   COLPRECONDITION( EnumIndex < pMember->Enumeration.size() );
   return pMember->Enumeration[ EnumIndex ];
}

COLuint32 XMLschemaEnumeration::countOfValues() const
{
   return pMember->Enumeration.size();
}

void XMLschemaEnumeration::addValue( const COLstring& Value )
{
   pMember->Enumeration.push_back( Value );
}

//collection
struct XMLschemaCollectionPrivate
{
   //by default, a collection's main node is a sequence
   XMLschemaCollectionPrivate()
   {
      MainNode = new XMLschemaSequence();
   }
   virtual ~XMLschemaCollectionPrivate()
   {
      delete MainNode;
   };

   XMLschemaNode* MainNode;
};

XMLschemaCollection::XMLschemaCollection( const COLstring& Name ) : XMLschemaType(  Name )
{
   pMember = new XMLschemaCollectionPrivate();
}

XMLschemaCollection::XMLschemaCollection(): XMLschemaType( "" )
{
   pMember = new XMLschemaCollectionPrivate();
}

XMLschemaCollection::~XMLschemaCollection()
{
   delete pMember;
}

const XMLschemaNode& XMLschemaCollection::mainNode() const
{
   return *pMember->MainNode;
}

XMLschemaNode& XMLschemaCollection::mainNode()
{
   return *pMember->MainNode;
}

COLuint32 XMLschemaCollection::countOfNodes() const
{
   COLPRECONDITION( pMember->MainNode->nodeType() == XMLschemaNode::eSequence );
   XMLschemaSequence* pSequence = (XMLschemaSequence*)(pMember->MainNode);
   return pSequence->countOfNodes();
}

void XMLschemaCollection::attachElement( XMLschemaElement* pElement )
{
   COLPRECONDITION( pMember->MainNode->nodeType() == XMLschemaNode::eSequence );
   XMLschemaSequence* pSequence = (XMLschemaSequence*)(pMember->MainNode);  
   pSequence->attachNode( pElement );
}

void XMLschemaCollection::attachSequence( XMLschemaSequence* pSequence )
{
   COLPRECONDITION( pMember->MainNode->nodeType() == XMLschemaNode::eSequence );
   XMLschemaSequence* m_pSequence = (XMLschemaSequence*)(pMember->MainNode);  
   m_pSequence->attachNode( pSequence );
}

void XMLschemaCollection::attachElementReference( XMLschemaReference* pReference )
{
   COLPRECONDITION( pMember->MainNode->nodeType() == XMLschemaNode::eSequence );
   XMLschemaSequence* pSequence = (XMLschemaSequence*)(pMember->MainNode);  
   pSequence->attachNode( pReference );
}
