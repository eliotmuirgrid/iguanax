//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UTFstring16
//
// Description:
//
// Implementation
//
// Author: Dmitri Klimenko
// Date:   Tuesday, February 10th, 2009 @ 06:15:31 PM
//
//---------------------------------------------------------------------------
#include "UTFprecomp.h"
#pragma hdrstop

#include "UTFstring16.h"

UTFstring16::UTFstring16():
   m_Data(0),
   m_Size(0)
{
};

UTFstring16::~UTFstring16()
{
   clear();
}

void UTFstring16::clear()
{
   if(m_Data)
   {
      delete [] m_Data;
      m_Data = 0;
      m_Size = 0;
   }
};

const UTF16* UTFstring16::data() const
{
   return m_Data;
}

UTF16* UTFstring16::data()
{
   return m_Data;
}

unsigned int UTFstring16::size() const
{
   return m_Size;
}

void UTFstring16::setWcharData(const wchar_t* Data, unsigned int Size)
{
   clear();

   m_Data = new UTF16[Size+1];
   for(COLindex CharIndex=0; CharIndex < Size; CharIndex++)
   {
      m_Data[CharIndex] = Data?Data[CharIndex]:0;
   }
   m_Data[Size] = 0;
   m_Size = Size;
}

void UTFstring16::setUtf16Data(const UTF16* Data, unsigned int Size)
{
   clear();

   m_Data = new UTF16[Size+1];
   for(COLindex CharIndex=0; CharIndex < Size; CharIndex++)
   {
      m_Data[CharIndex] = Data?Data[CharIndex]:0;
   }
   m_Data[Size] = 0;
   m_Size = Size;
}
