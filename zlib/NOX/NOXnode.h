#ifndef __NOX_NODE_H__
#define __NOX_NODE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOXnode
//
// Description:
//
// NOXnode class.  Represents an element, or the root document.
//
// Author: Kevin Senn
// Date:   Tuesday, June 22nd, 2010 @ 03:05:08 PM
//
//---------------------------------------------------------------------------

#include <COL/COLref.h>
#include <COL/COLlist.h>
#include <COL/COLhashmap.h>
#include <COL/COLauto.h>

#include "NOXconstant.h"
#include "NOXcontext.h"

#include <NOD/NODplaceComposite.h>

#include <DOM/DOMdocument.h>

class NOXnode : public NODplaceComposite
{
public:
   NOXnode(DOMnode* pDomNode, NOXcontext* pContext);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~NOXnode();

   NODgrammarType grammarType() const { return NOD_COMPLEX; }

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

   // Override some NODplace methods.
   void clear();
   NODplace& add(const char* Arg1, const char* Arg2);
   NODplace& insert(int i, const char* Arg1, const char* Arg2);
   void nullify(int i);
   void nullify(const COLstring& Name);

protected:
   // Not used, since we override add() and insert().
   NODplace* newChild(int i, const char* Arg1, const char* Arg2) { COLASSERT(false); return NULL; }

   // Not used, since we override nullify() and clear().
   void onNullifyChild(int i) { COLASSERT(false); }

   NODplace* initChild(int i) const;
   bool onRemoveChild(int i);
   
private:
   mutable COLref<DOMnode> m_pDomNode;
   mutable COLint64 m_NameId;
   mutable COLref<NOXcontext> m_pContext;

   // Owned by this object.  Only used if setTreeValue() is called on this node.
   // Necessary to keep the string data for all nodes parsed from the raw XML.
   // TODO this takes up unnecessary space for simple nodes.
   COLlist<DOMdocument*> m_pParsedDocumentz;

   void resetSize();

   NOXnode(const NOXnode& Orig); // not allowed
   NOXnode& operator=(const NOXnode& Orig); // not allowed
};

#endif // end of defensive include
