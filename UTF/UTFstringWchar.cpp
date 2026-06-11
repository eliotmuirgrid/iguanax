//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UTFstringWchar
//
// Description:
//
// Implementation
//
// Author: Dmitri Klimenko
// Date:   Tuesday, February 10th, 2009 @ 05:05:23 PM
//
//---------------------------------------------------------------------------
#include "UTFprecomp.h"
#pragma hdrstop

#include "UTFstringWchar.h"

UTFstringWchar::UTFstringWchar():
   m_Data(0),
   m_Size(0)
{
};

UTFstringWchar::~UTFstringWchar()
{
   clear();
}

void UTFstringWchar::clear()
{
   if(m_Data)
   {
      delete [] m_Data;
      m_Data = 0;
      m_Size = 0;
   }
};

const wchar_t* UTFstringWchar::data() const
{
   return m_Data;
}

wchar_t* UTFstringWchar::data()
{
   return m_Data;
}

unsigned int UTFstringWchar::size() const
{
   return m_Size;
}

void UTFstringWchar::setWcharData(const wchar_t* Data, unsigned int Size)
{
   clear();

   m_Data = new wchar_t[Size+1];
   for(COLindex CharIndex=0; CharIndex < Size; CharIndex++)
   {
      m_Data[CharIndex] = Data?Data[CharIndex]:0;
   }
   m_Data[Size] = 0;
   m_Size = Size;
}

void UTFstringWchar::setUtf16Data(const UTF16* Data, unsigned int Size)
{
   clear();

   m_Data = new wchar_t[Size+1];
   for(COLindex CharIndex=0; CharIndex < Size; CharIndex++)
   {
      m_Data[CharIndex] = Data?Data[CharIndex]:0;
   }
   m_Data[Size] = 0;
   m_Size = Size;
}
