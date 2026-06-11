//---------------------------------------------------------------------------
// Copyright (C) 1997-2006 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLschemaProperties
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   December 29, 2003 
//
//---------------------------------------------------------------------------
#include "XMLprecomp.h"
#pragma hdrstop

#include "XMLschemaProperties.h"

void XMLschemaProperties::setXmlNamespace( const char* pNamespace )
{
   XmlNamespace = pNamespace;
}

const COLstring& XMLschemaProperties::xmlNamespace() const
{
   return XmlNamespace;
}
   
void XMLschemaProperties::setTargetNamespace( const char* pNamespace )
{
   TargetNamespace = pNamespace;
}

const COLstring& XMLschemaProperties::targetNamespace() const
{
   return TargetNamespace;
}

void XMLschemaProperties::setElementFormDefault( const char* pElementFormDefault )
{
   ElementFormDefault = pElementFormDefault;
}

const COLstring& XMLschemaProperties::elementFormDefault() const
{
   return ElementFormDefault;
}
