#ifndef __COL_REF_COUNTED_H__
#define __COL_REF_COUNTED_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: Definition of COLrefCounted object
//
// Description:
//
// Parent class for all reference counted objects
//
// Author: Eliot Muir
//---------------------------------------------------------------------------

#include "COLminimumInclude.h"

class COLrefCounted {
public:
   COLrefCounted();
   virtual ~COLrefCounted();

   // not strictly adhering to our naming conventions but
   // necessary for compatibility with COM the Chameleon
   // COM wrapper stuff.
   void AddRef();
   void Release();

   int refcount() const { return (int)m_CountOfRef; }
protected:
   size_t m_CountOfRef;
//private:
   //COLrefCounted(const COLrefCounted&);            // disallow
   //COLrefCounted& operator=(const COLrefCounted&); // disallow
};

#endif // end of defensive include
