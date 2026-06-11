#ifndef __UTF_STRING_WCHAR_H__
#define __UTF_STRING_WCHAR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UTFstringWchar
//
// Description:
//
// UTFstringWchar class.
//
// Author: Dmitri Klimenko
// Date:   Tuesday, February 10th, 2009 @ 05:05:23 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "ConvertUTF.h"

class UTFstringWchar 
{
public:
   UTFstringWchar();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~UTFstringWchar();

   const wchar_t* data() const;
   wchar_t* data();
   unsigned int size() const;

   void setWcharData(const wchar_t* Data, unsigned int Size);
   void setUtf16Data(const UTF16* Data, unsigned int Size);

   void clear();

private:
   wchar_t* m_Data;
   unsigned int m_Size;

   UTFstringWchar(const UTFstringWchar& Orig); // not allowed
   UTFstringWchar& operator=(const UTFstringWchar& Orig); // not allowed
};

#endif // end of defensive include
