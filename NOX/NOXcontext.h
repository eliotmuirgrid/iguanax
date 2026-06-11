#ifndef __NOX_CONTEXT_H__
#define __NOX_CONTEXT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXcontext
//
// Description:
//
// NOXcontext class.
//
// Author: Kevin Senn
// Date:   Monday, September 19th, 2011 @ 11:06:10 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLref.h>

#include <NOD/NODmemoizer.h>

#include <DOM/DOMnode.h>

class NOXcontext : public NODmemoizer
{
public:
   NOXcontext(DOMnode* pOwnerDocument) : m_pOwnerDocument(pOwnerDocument) {}
   // This is non virtual because we are assuming no classes will inherit from this class
   ~NOXcontext() {}

private:
   COLref<DOMnode> m_pOwnerDocument; // so it doesn't get deallocated.

   NOXcontext(const NOXcontext& Orig); // not allowed
   NOXcontext& operator=(const NOXcontext& Orig); // not allowed
};

#endif // end of defensive include
