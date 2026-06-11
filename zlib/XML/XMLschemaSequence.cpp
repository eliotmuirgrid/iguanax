//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLschemaSequence
//
// Description:
//
// This class represents an sequence in the schema
//
// Author: Nasron Cheong
// Date:   Nov 27 2003 
//
//---------------------------------------------------------------------------
#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLschemaSequence.h>
#include <XML/XMLschemaType.h>
#include <XML/XMLschema.h>
#include <XML/XMLschemaPointer.h>
#include <LEG/LEGvector.h>

struct XMLschemaSequencePrivate
{
   XMLschemaSequencePrivate( ){}
   ~XMLschemaSequencePrivate(){}
      
   LEGvector< XMLschemaPointer< XMLschemaNode > > Nodes;
};

XMLschemaSequence::XMLschemaSequence()
{
   pMember = new XMLschemaSequencePrivate();
}

XMLschemaSequence::~XMLschemaSequence()
{
   delete pMember;
}

XMLschemaNode& XMLschemaSequence::nodeAt( COLuint32 NodeIndex )
{
   COLPRECONDITION( NodeIndex < pMember->Nodes.size() );
   return *pMember->Nodes[NodeIndex];
}

const XMLschemaNode& XMLschemaSequence::nodeAt( COLuint32 NodeIndex ) const
{
   COLPRECONDITION( NodeIndex < pMember->Nodes.size() );
   return *pMember->Nodes[NodeIndex];
}

COLuint32 XMLschemaSequence::countOfNodes() const
{
   return pMember->Nodes.size();
}

void XMLschemaSequence::attachNode( XMLschemaPointer<XMLschemaNode> pNode )
{
   pMember->Nodes.push_back(pNode);
}
