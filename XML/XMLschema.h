#ifndef __XML_SCHEMA_H__
#define __XML_SCHEMA_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLschema
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

class XMLschemaElement;
class XMLschemaType;
class COLstring;
class COLostream;

struct XMLschemaPrivate;
class XMLschema
{
public:
   XMLschema();
   virtual ~XMLschema();
  
   //type collection
   XMLschemaType& typeAt( COLuint32 Index );
   const XMLschemaType& typeAt( COLuint32 Index ) const;
   void attachType( XMLschemaType* pType ); //takes ownership
   COLuint32 countOfTypes() const;
   XMLschemaType* findType( const COLstring& Name ); //returns 0 if not found

   //element collection
   XMLschemaElement& elementAt( COLuint32 Index );
   const XMLschemaElement& elementAt( COLuint32 Index ) const;
   void attachElement( XMLschemaElement* pElement ); //takes ownership
   COLuint32 countOfElements() const;
   XMLschemaElement* findElement( const COLstring& Name ); //returns 0 if not found
   
private:
   XMLschema(const XMLschema& Original);
   XMLschema& operator=(const XMLschema& Original);
   XMLschemaPrivate* pMember;
};


#endif // end of defensive include





















