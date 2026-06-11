#ifndef __XML_SCHEMA_TYPE_H__
#define __XML_SCHEMA_TYPE_H__
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

#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>

class COLstring;
class XMLschemaNode;
class XMLschemaSequence;
class XMLschemaElement;
class XMLschemaReference;
class XMLschemaEnumeration;
class XMLschemaCollection;

struct XMLschemaTypePrivate;
class XMLschemaType
{
public:
   enum EType
   {
      eSimple,
      eEnumeration,
      eCollection
   };
   
   XMLschemaType( const COLstring& Name_ ) : Name( Name_ ){}
   virtual ~XMLschemaType(){}

   virtual EType id() const = 0;
   virtual const COLstring& name() const { return Name; }

private:
   XMLschemaType(const XMLschemaType& Original);
   XMLschemaType& operator=(const XMLschemaType& Original);
   COLstring Name;
};

class XMLschemaSimple : public XMLschemaType
{
public:
   enum ESimpleType
   {
      eString,
      eInteger,
      eDouble,
      eDateTime   
   };

   XMLschemaSimple(ESimpleType iElementType ) 
      : XMLschemaType( "" ), ElementType(iElementType){}
   virtual ~XMLschemaSimple(){}


   virtual EType id() const{ return XMLschemaType::eSimple; }
   ESimpleType simpleId() const {return ElementType; }

   static XMLschemaSimple String;
   static XMLschemaSimple Integer;
   static XMLschemaSimple Double;
   static XMLschemaSimple DateTime;

private:
   ESimpleType ElementType;
   XMLschemaSimple(const XMLschemaSimple& Original);
   XMLschemaSimple& operator=(const XMLschemaSimple& Original);
};

struct XMLschemaEnumerationPrivate;
class XMLschemaEnumeration : public XMLschemaType
{
public:
   XMLschemaEnumeration();
   XMLschemaEnumeration( const COLstring& Name );
   virtual ~XMLschemaEnumeration();

   virtual EType id() const{ return XMLschemaType::eEnumeration; }
   COLstring& valueAt(COLuint32 EnumIndex);
   const COLstring& valueAt(COLuint32 EnumIndex) const;
   void addValue( const COLstring& Value );
   COLuint32 countOfValues() const;


private:
   XMLschemaEnumeration(const XMLschemaEnumeration& Original);
   XMLschemaEnumeration& operator=(const XMLschemaEnumeration& Original);
   XMLschemaEnumerationPrivate* pMember;

};

struct XMLschemaCollectionPrivate;
class XMLschemaCollection : public XMLschemaType
{
public:
   XMLschemaCollection();
   XMLschemaCollection( const COLstring& Name );
   virtual ~XMLschemaCollection();
   
   virtual EType id() const{ return XMLschemaType::eCollection; }
   const XMLschemaNode& mainNode() const;
   XMLschemaNode& mainNode();

   //takes ownership
   void attachElement( XMLschemaElement* pElement ); 
   void attachElementReference( XMLschemaReference* pReference ); 
   void attachSequence( XMLschemaSequence* pSequence ); 
   COLuint32 countOfNodes() const;

private:
   XMLschemaCollection(const XMLschemaCollection& Original);
   XMLschemaCollection& operator=(const XMLschemaCollection& Original);
   XMLschemaCollectionPrivate* pMember;
};


#endif // end of defensive include
