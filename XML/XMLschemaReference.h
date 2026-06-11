#ifndef __XML_SCHEMA_REFERENCE_H__
#define __XML_SCHEMA_REFERENCE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLschemaReference
//
// Description:
//
// This class represents an reference in the schema
//
// Author: Nasron Cheong
// Date:   Sept 2007 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <XML/XMLschemaNode.h>

class COLstring;
class XMLschemaType;

class XMLschemaReference : public XMLschemaNode
{
public:
   //support element references only for now
   XMLschemaReference( XMLschemaElement& ReferencedElement )
      : pReferencedElement(&ReferencedElement), XMLschemaNode()
   {
   }
   virtual ~XMLschemaReference()
   {
   }

   virtual const XMLschemaNode::EschemaNodeType nodeType() const {return XMLschemaNode::eReference;}

   XMLschemaElement& referencedElement(){return *pReferencedElement;}
   const XMLschemaElement& referencedElement() const{return *pReferencedElement;}

private:
   XMLschemaElement* pReferencedElement;
   XMLschemaReference(const XMLschemaReference& Original); // not allowed
   XMLschemaReference& operator=(const XMLschemaReference& Original);  // not allowed
};


#endif // end of defensive include





















