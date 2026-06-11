#ifndef __XML_SCHEMA__
#define __XML_SCHEMA__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLxsdSchema
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

#include <COL/COLminimumInclude.h>
#include <XML/XMLschema.h>

class COLostream;

class XMLxsdSchemaFormatter
{
public:
   XMLxsdSchemaFormatter();
   virtual ~XMLxsdSchemaFormatter();
  
   virtual void printOn( XMLschemaCOLostream& Stream ); 

private:
   struct XMLstream;
   void printCollectionOn( XMLschemaCollection& Collection, XMLstream& Stream );

   XMLxsdSchema(const XMLxsdSchema& Original);
   XMLxsdSchema& operator=(const XMLxsdSchema& Original);
};
#endif // end of defensive include
