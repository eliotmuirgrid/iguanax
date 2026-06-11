#ifndef __XML_SCHEMA_SEQUENCE_H__
#define __XML_SCHEMA_SEQUENCE_H__
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

#include <COL/COLminimumInclude.h>
#include <XML/XMLschemaNode.h>
#include <XML/XMLschemaPointer.h>

class COLstring;
class XMLschemaType;

struct XMLschemaSequencePrivate;

class XMLschemaSequence : public XMLschemaNode
{
public:
   XMLschemaSequence();
   virtual ~XMLschemaSequence();

   virtual const XMLschemaNode::EschemaNodeType nodeType() const {return XMLschemaNode::eSequence;}

   void attachNode( XMLschemaPointer<XMLschemaNode> pNode ); //takes ownership
   XMLschemaNode& nodeAt( COLuint32 Index );
   const XMLschemaNode& nodeAt( COLuint32 Index ) const;  
   COLuint32 countOfNodes() const;

private:
   XMLschemaSequence(const XMLschemaSequence& Original); // not allowed
   XMLschemaSequence& operator=(const XMLschemaSequence& Original);  // not allowed
   XMLschemaSequencePrivate* pMember;
};


#endif // end of defensive include













