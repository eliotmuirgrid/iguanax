//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLschemaElement
//
// Description:
//
// This class represents an element in the schema
//
// Author: Ryan Dewsbury
// Date:   Mon 05/28/2001 
//
//---------------------------------------------------------------------------
#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLschemaElement.h>
#include <XML/XMLschemaType.h>
#include <XML/XMLschema.h>

const COLuint32 XMLschemaNode::OccursUnbounded = (COLuint32)-1;

struct XMLschemaElementPrivate
{
   XMLschemaElementPrivate( ){}
   ~XMLschemaElementPrivate(){}
      
   COLstring Name;
   XMLschemaPointer< XMLschemaType > pType;
};

XMLschemaElement::XMLschemaElement( const COLstring& Name, XMLschemaType* pType, bool Reference )
{
   pMember = new XMLschemaElementPrivate();
   pMember->Name = Name;
   pMember->pType = XMLschemaPointer< XMLschemaType >( pType, !Reference );
}

XMLschemaElement::~XMLschemaElement()
{
   delete pMember;
}

const COLstring& XMLschemaElement::name() const
{
   return pMember->Name;
}

void XMLschemaElement::setName( const COLstring& Name )
{
   pMember->Name = Name;
}

XMLschemaType& XMLschemaElement::type() const
{
   return *pMember->pType;
}

bool XMLschemaElement::isTypeReference() const
{
   return !pMember->pType.isOwner();
}

















