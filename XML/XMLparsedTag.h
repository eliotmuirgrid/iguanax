#ifndef __XML_PARSED_TAG_H__
#define __XML_PARSED_TAG_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: XMLparsedTag
//
// Description:
//
// This class represents a parsed XML tag. This unfinished
// and therefore really obsolete.
//
// Author: Eliot Muir
// Date:   Sun 06/03/2001 
//
//---------------------------------------------------------------------------

#include <COL/COLrefCounted.h>

class XMLparsedTagPrivate;
class COLstring;
class XMLtag;

class XMLparsedTag : public COLrefCounted
{
public:
   XMLparsedTag(const COLstring& Name="");
   virtual ~XMLparsedTag();

   void clear();

   const COLstring& name() const;
   void setName(const COLstring& Name);

   size_t countOfSubTag() const;

   const XMLparsedTag& subTag(size_t TagIndex) const;
   XMLparsedTag& subTag(size_t TagIndex);

   void addTag(XMLparsedTag* pTag);

   void removeTag(size_t TagIndex);

   const COLstring& data() const;
   void setData(const COLstring& Data);

   XMLparsedTag* parent();
   const XMLparsedTag* parent() const;

   void printOn(COLostream& Stream, const COLstring& Indent="") const;
private:
   XMLparsedTagPrivate* pMember;
   // not allowed
   XMLparsedTag(const XMLparsedTag& Orig);
   XMLparsedTag& operator=(const XMLparsedTag& Orig);
};

COLostream& operator<<(COLostream& Stream, const XMLparsedTag& Dtd);

#endif // end of defensive include


