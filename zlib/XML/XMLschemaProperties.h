#ifndef __XML_SCHEMA_PROPERTIES_H__
#define __XML_SCHEMA_PROPERTIES_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLschemaProperties
//
// Description:
//
// This class is used to avoid keeping state on the formatters, and is usually
// passed into the schema formatter as a parameter
//
// Author: Nasron Cheong
// Date:   December 29, 2003 
//
//---------------------------------------------------------------------------

class XMLschemaPropertiesPrivate; 
class COLstring;

class XMLschemaProperties
{
public:
   XMLschemaProperties(){}
   ~XMLschemaProperties(){}
   // POD so the default assignment and copy constructors work

   void setXmlNamespace( const char* pNamespace );
   const COLstring& xmlNamespace() const;
   
   void setTargetNamespace( const char* pNamespace );
   const COLstring& targetNamespace() const;

   void setElementFormDefault( const char* pElementFormDefault ); 
   const COLstring& elementFormDefault() const;
   
private:
   COLstring XmlNamespace;
   COLstring TargetNamespace;
   COLstring ElementFormDefault;
};

#endif // end of defensive include
