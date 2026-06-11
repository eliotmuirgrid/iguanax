#ifndef __XML_SCHEMA_ELEMENT_H__
#define __XML_SCHEMA_ELEMENT_H__
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

#include <COL/COLminimumInclude.h>
#include <XML/XMLschemaNode.h>
#include <XML/XMLschemaPointer.h>

class COLstring;
class XMLschemaType;

struct XMLschemaElementPrivate;
class XMLschemaElement : public XMLschemaNode
{
public:
   XMLschemaElement( const COLstring& Name, XMLschemaType* pType, bool Reference );
   virtual ~XMLschemaElement();

   virtual const XMLschemaNode::EschemaNodeType nodeType() const {return XMLschemaNode::eElement;}

   const COLstring& name() const;
   void setName( const COLstring& Name );

   XMLschemaType& type() const;
   bool isTypeReference() const;

private:
   XMLschemaElement(const XMLschemaElement& Original); // not allowed
   XMLschemaElement& operator=(const XMLschemaElement& Original);  // not allowed
   XMLschemaElementPrivate* pMember;
};


#endif // end of defensive include





















