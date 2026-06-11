//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBsimpleEmpty
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Friday, September 16th, 2011 @ 03:04:18 PM
//
//---------------------------------------------------------------------------
#include "NOBprecomp.h"
#pragma hdrstop

#include "NOBsimpleEmpty.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#define NOB_MASK_NAMED        1
#define NOB_MASK_ESCAPE_VALUE 2
#define NOB_MASK_IS_NULL      4

NOBsimpleEmpty::NOBsimpleEmpty(NOBmemoizer* pMemoizer)
 : m_Flags(NOB_MASK_NAMED | NOB_MASK_ESCAPE_VALUE),
   m_NodeType(-1),
   m_ProtocolType(-1),
   m_pMemoizer(pMemoizer)
{
   COLASSERT(pMemoizer);
}

const COLstring& NOBsimpleEmpty::protocolName() const{
   return m_pMemoizer->protocol(m_ProtocolType);
}

const COLstring& NOBsimpleEmpty::nodeTypeName() const{
   return m_pMemoizer->nodeType(m_ProtocolType, m_NodeType);
}

const COLstring& NOBsimpleEmpty::name() const{
   return m_pMemoizer->valueFromId(m_NameId);
}

bool NOBsimpleEmpty::isNull() const{
   return m_Flags & NOB_MASK_IS_NULL;
}

bool NOBsimpleEmpty::isNamed() const{
   return m_Flags & NOB_MASK_NAMED;
}

bool NOBsimpleEmpty::shouldEscapeValue() const{
   return m_Flags & NOB_MASK_ESCAPE_VALUE;
}

void NOBsimpleEmpty::setName(const COLstring& Name){
   m_NameId = m_pMemoizer->idFromValue(Name);
}

COLint8 NOBgenerateSimpleFlags(const NODplace& Place){
   return
      (Place.isNamed()           ? NOB_MASK_NAMED        : 0) |
      (Place.shouldEscapeValue() ? NOB_MASK_ESCAPE_VALUE : 0) |
      (Place.isNull()            ? NOB_MASK_IS_NULL      : 0);
}
