#ifndef __NOB_COMPOSITE_H__
#define __NOB_COMPOSITE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBcomposite
//
// Description:
//
// NOBcomposite class.  Represents a copied/reconstructed NODplace (complex
// or vector type).
//
// Author: Kevin Senn
// Date:   Thursday, September 15th, 2011 @ 09:40:08 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include <NOD/NODplaceComposite.h>

#include "NOBmemoizer.h"

class NOBcomposite : public NODplaceComposite
{
public:
   NOBcomposite(NOBmemoizer* pMemoizer);
   virtual ~NOBcomposite() {}

   NODgrammarType grammarType() const;
   int protocolType() const { return m_ProtocolType; }
   int nodeType() const { return m_NodeType; }
   const COLstring& protocolName() const;
   const COLstring& nodeTypeName() const;

   const COLstring& name() const;

   bool isNull() const;

   bool isReadOnly() const { return true; }
   bool isNamed() const;
   bool isHomogeneous() const;
   bool favorIndexByName() const;
   bool gracefulNoSuchMember() const;

   // These functions are not part of the NODplace interface.
   
   void setNodeType(int NodeType)         { m_NodeType = NodeType; }
   void setProtocolType(int ProtocolType) { m_ProtocolType = ProtocolType; }
   
   void setName(const COLint64 NameId) { m_NameId = NameId; }
   void setName(const COLstring& Name);

   void setFlags(COLint8 Flags) { m_Flags = Flags; }

   // Both these functions will increase the NODplace's reference count.
   NODplace* addChild(NODplace* pChild);
   NODplace* initDummyItem(NODplace* pDummy);

   NOBmemoizer* memoizer() { return m_pMemoizer.get(); }
   const NOBmemoizer* memoizer() const { return m_pMemoizer.get(); }

protected:
   // An index of -1 indicates that a "dummy" item is being created for
   // a homogeneous vector (see NODplace::createDummyItem()).
   NODplace* newChild(int i, const char* Arg1, const char* Arg2);

   NODplace* initChild(int i) const { COLASSERT(false); return NULL; }

private:
   // If we wanted to get really fancy we could combine these two fields into a single int.
   // For example, NodeType + N*ProtocolType.  As long as N is higher than the highest possible NodeType.
   int m_NodeType;
   int m_ProtocolType;

   COLint64 m_NameId; // memoized

   COLint8 m_Flags;

   // A dummy item node, if this node has one.
   COLref<NODplace> m_pDummyItem;

   COLref<NOBmemoizer> m_pMemoizer;

   NOBcomposite(const NOBcomposite& Orig); // not allowed
   NOBcomposite& operator=(const NOBcomposite& Orig); // not allowed
};

// Generate a flags field for a NODplace (for serialization).
COLint8 NOBgenerateCompositeFlags(const NODplace& Place);

#endif // end of defensive include
