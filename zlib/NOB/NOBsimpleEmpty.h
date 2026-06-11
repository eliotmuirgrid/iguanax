#ifndef __NOB_SIMPLE_EMPTY_H__
#define __NOB_SIMPLE_EMPTY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBsimpleEmpty
//
// Description:
//
// NOBsimpleEmpty class.  A simple NODplace, with an empty string value.
//
// Author: Kevin Senn
// Date:   Friday, September 16th, 2011 @ 03:04:18 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include <NOD/NODsimple.h>

#include "NOBmemoizer.h"

class NOBsimpleEmpty : public NODsimple
{
public:
   NOBsimpleEmpty(NOBmemoizer* pMemoizer);
   
   virtual ~NOBsimpleEmpty() {}

   int protocolType() const { return m_ProtocolType; }
   int nodeType() const { return m_NodeType; }
   const COLstring& protocolName() const;
   const COLstring& nodeTypeName() const;

   const COLstring& name() const;

   bool isNull() const;

   bool isReadOnly() const { return true; }
   bool isNamed() const;

   void setValue(const COLstring& Value) { COLASSERT(!isReadOnly()); }
   const COLstring& value() const { return NODplace::NOD_EMPTY_SIMPLE_VALUE; }
   bool shouldEscapeValue() const;

   // These functions are not part of the NODplace interface.

   void setNodeType(int NodeType)         { m_NodeType = NodeType; }
   void setProtocolType(int ProtocolType) { m_ProtocolType = ProtocolType; }

   void setName(const COLint64 NameId) { m_NameId = NameId; }
   void setName(const COLstring& Name);

   void setFlags(COLint8 Flags) { m_Flags = Flags; }

   NOBmemoizer* memoizer() { return m_pMemoizer.get(); }
   const NOBmemoizer* memoizer() const { return m_pMemoizer.get(); }

private:
   // If we wanted to get really fancy we could combine these two fields into a single int.
   // For example, NodeType + N*ProtocolType.  As long as N is higher than the highest possible NodeType.
   int m_NodeType;
   int m_ProtocolType;

   COLint64 m_NameId; // memoized

   COLint8 m_Flags;

   COLref<NOBmemoizer> m_pMemoizer;

   NOBsimpleEmpty(const NOBsimpleEmpty& Orig); // not allowed
   NOBsimpleEmpty& operator=(const NOBsimpleEmpty& Orig); // not allowed
};

COLint8 NOBgenerateSimpleFlags(const NODplace& Place);

#endif // end of defensive include
