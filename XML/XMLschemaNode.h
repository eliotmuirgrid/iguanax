#ifndef __XML_SCHEMA_NODE_H__
#define __XML_SCHEMA_NODE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLschemaNode
//
// Description:
//
// Base class for all schema nodes
//
// Author: Nasron Cheong
// Date:   Nov 27 2003 
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLstring;
class XMLschemaType;
class XMLschemaNodePrivate;

class XMLschemaNode
{
public:
   XMLschemaNode() : MinOccurs(1), MaxOccurs(1){};
   virtual ~XMLschemaNode(){};

   enum EschemaNodeType
   {
      eElement,
      eReference,
      eSequence
   };

   static const COLuint32 OccursUnbounded;

   virtual const EschemaNodeType nodeType() const = 0; 

   COLuint32 minOccurs() const{return MinOccurs;}
   void setMinOccurs( COLuint32 iMinOccurs ){MinOccurs = iMinOccurs;}

   COLuint32 maxOccurs() const {return MaxOccurs;}
   void setMaxOccurs( COLuint32 iMaxOccurs ){MaxOccurs = iMaxOccurs;}

private:
   COLuint32 MinOccurs;
   COLuint32 MaxOccurs;
   XMLschemaNode(const XMLschemaNode& Original); // not allowed
   XMLschemaNode& operator=(const XMLschemaNode& Original);  // not allowed
};
#endif // end of defensive include
