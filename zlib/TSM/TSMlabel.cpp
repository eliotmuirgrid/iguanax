//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMlabel
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, February 12th, 2007 @ 09:33:05 AM
//
//---------------------------------------------------------------------------
#include "TSMprecomp.h"
#pragma hdrstop

#include "TSMlabel.h"

#include <string.h>

TSMlabel::TSMlabel()
: Id(0),
  pLabel(""),
  SizeOfLabel(0)
{
}

TSMlabel::TSMlabel(const COLstring& String)
: Id(0),
  pLabel(String.c_str()),
  SizeOfLabel(String.size())
{      
}

TSMlabel::TSMlabel(const char* pString)
: Id(0),
  pLabel(pString),
  SizeOfLabel(strlen(pString))
{      
}

TSMlabel::TSMlabel(int iId, const char* pString)
: Id(iId),
  pLabel(pString),
  SizeOfLabel(strlen(pString))
{      
}

TSMlabel::TSMlabel(int iId, const char* pString, COLuint32 Size)
: Id(iId),
  pLabel(pString),
  SizeOfLabel(Size)
{   
}

TSMlabel::~TSMlabel()
{
}

void TSMlabel::setString(const char* pLabel)
{
   setStringWithSize(pLabel, strlen(pLabel));
}

void TSMlabel::setStringWithSize(const char* pString, COLuint32 Size)
{
   pLabel = pString;
   SizeOfLabel = Size;
}

void TSMlabel::setId(int iId)
{
   Id = iId;
}

bool TSMlabel::operator==(const TSMlabel& Rhs) const
{
   if(id() == Rhs.id())
   {
      return true;
   }
   if(size() != Rhs.size())
   {
      return false;
   }
   if(memcmp(label(), Rhs.label(), size()) != 0)
   {
      return false;
   }
   return true;
}

bool TSMlabel::operator==(const char* pRhs) const
{
   COLuint32 Size = strlen(pRhs);
   if(size() != Size)
   {
      return false;
   }
   if(memcmp(label(), pRhs, Size) != 0)
   {
      return false;
   }
   return true;
}

bool TSMlabel::operator!=(const TSMlabel& Rhs) const
{
   return !operator==(Rhs);
}

bool TSMlabel::operator!=(const char* pRhs) const
{
   return !operator==(pRhs);
}

COLostream& operator<<(COLostream& Stream, const TSMlabel& Label)
{
   Stream.sink()->write(Label.label(), Label.size());
   return Stream;
}
