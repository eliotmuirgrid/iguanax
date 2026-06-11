//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLparsedTag
//
// Description:
//
// Implementation of XMLparsedTag
//
// Author: Eliot Muir
// Date:   Sun 06/03/2001 
//
//---------------------------------------------------------------------------

#include "XMLprecomp.h"
#pragma hdrstop

#include <XML/XMLparsedTag.h>
#include <LEG/LEGvector.h>
#include <COL/COLref.h>

class XMLparsedTagPrivate
{
public:
   XMLparsedTagPrivate(const COLstring& iName) 
      : Name(iName), pParent(NULL) {}   
   ~XMLparsedTagPrivate() {}

   COLstring Name;
   COLstring Data;

   XMLparsedTag* pParent;

   LEGvector< COLref<XMLparsedTag> > SubTagVector;
};

XMLparsedTag::XMLparsedTag(const COLstring& Name) : pMember(NULL)
{
   pMember = new XMLparsedTagPrivate(Name);
}

XMLparsedTag::~XMLparsedTag()
{
   delete pMember;
}

void XMLparsedTag::addTag(XMLparsedTag* pTag)
{
   pMember->SubTagVector.push_back(pTag);
   pTag->pMember->pParent = this;
}

void XMLparsedTag::removeTag(size_t TagIndex)
{
   pMember->SubTagVector.remove(TagIndex);
}

void XMLparsedTag::clear()
{
   pMember->SubTagVector.clear();
}

const COLstring& XMLparsedTag::name() const
{
   return pMember->Name;
}

void XMLparsedTag::setName(const COLstring& Name)
{
   pMember->Name = Name;
}

size_t XMLparsedTag::countOfSubTag() const
{
   return pMember->SubTagVector.size();
}

const XMLparsedTag& XMLparsedTag::subTag(size_t TagIndex) const
{
   return *pMember->SubTagVector[TagIndex];
}

XMLparsedTag& XMLparsedTag::subTag(size_t TagIndex)
{
   return *pMember->SubTagVector[TagIndex];
}

const COLstring& XMLparsedTag::data() const
{
   return pMember->Data;
}

void XMLparsedTag::setData(const COLstring& Data)
{
   pMember->Data = Data;
}

void XMLparsedTag::printOn(COLostream& Stream, const COLstring& Indent) const
{   
   Stream << Indent << '<' << name() << '>';

   if (countOfSubTag() > 0)
   {
      Stream << newline << flush;
      for (size_t TagIndex = 0;
           TagIndex < countOfSubTag();
           TagIndex++)
      {
         subTag(TagIndex).printOn(Stream, Indent + "   ");
      }
      
      Stream << Indent << COL_T("</") << name()<< '>' << newline << flush;
   }
   else
   {
      Stream << data() << COL_T("</") << name() <<  '>' << newline << flush;
   }
}

XMLparsedTag* XMLparsedTag::parent()
{
   return pMember->pParent;
}

const XMLparsedTag* XMLparsedTag::parent() const
{
   return pMember->pParent;
}

COLostream& operator<<(COLostream& Stream, const XMLparsedTag& Tag)
{
   if (Tag.countOfSubTag() > 0)
   {
      Tag.subTag(0).printOn(Stream);
   }
   return Stream;
}
