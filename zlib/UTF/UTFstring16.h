#ifndef __UTF_STRING16_H__
#define __UTF_STRING16_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UTFstring16
//
// Description:
//
// UTFstring16 class.
//
// Author: Dmitri Klimenko
// Date:   Tuesday, February 10th, 2009 @ 06:15:31 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "ConvertUTF.h"

class UTFstring16 
{
public:
   UTFstring16();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~UTFstring16();

   const UTF16* data() const;
   UTF16* data();
   unsigned int size() const;

   void setWcharData(const wchar_t* Data, unsigned int Size);
   void setUtf16Data(const UTF16* Data, unsigned int Size);

   void clear();

private:
   UTF16* m_Data;
   unsigned int m_Size;

   UTFstring16(const UTFstring16& Orig); // not allowed
   UTFstring16& operator=(const UTFstring16& Orig); // not allowed
};

#endif // end of defensive include
