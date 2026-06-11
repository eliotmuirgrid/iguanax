#ifndef __NOX_SIMPLE_H__
#define __NOX_SIMPLE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXsimple
//
// Description:
//
// NOXsimple class.
//
// Author: Kevin Senn
// Date:   Monday, September 19th, 2011 @ 11:44:46 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include "NOXconstant.h"
#include "NOXcontext.h"

#include <NOD/NODsimple.h>

#include <DOM/DOMnode.h>

class NOXsimple : public NODsimple
{
public:
   NOXsimple(DOMnode* pDomNode, NOXcontext* pContext);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~NOXsimple() {}

   NODgrammarType grammarType() const { return NOD_SIMPLE; }

   int protocolType() const { return NOX_XML; }
   int nodeType() const;
   const COLstring& protocolName() const { return NOXprotocolName; }
   const COLstring& nodeTypeName() const;

   const COLstring& name() const;

   bool isReadOnly() const { return false; }
   bool isNamed() const;
   bool isHomogeneous() const { COLASSERT(isVector()); return false; }
   bool favorIndexByName() const { return true; }
   bool gracefulNoSuchMember() const { return true; }

   bool setTreeValue(const COLstring& TreeValue);

   bool isNull() const { return false; } // can't have a "non-present" XML node

   void setValue(const COLstring& Value);
   const COLstring& value() const { return m_Value; }

private:
   mutable COLref<DOMnode> m_pDomNode;
   mutable COLint64 m_NameId;
   mutable COLref<NOXcontext> m_pContext;
   COLstring m_Value;

   NOXsimple(const NOXsimple& Orig); // not allowed
   NOXsimple& operator=(const NOXsimple& Orig); // not allowed
};

#endif // end of defensive include
