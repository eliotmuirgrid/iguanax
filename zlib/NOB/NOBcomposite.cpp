//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBcomposite
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, September 15th, 2011 @ 09:40:08 AM
//
//---------------------------------------------------------------------------
#include "NOBprecomp.h"
#pragma hdrstop

#include "NOBcomposite.h"
#include "NOBsimple.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#define NOB_MASK_COMPLEX      1
#define NOB_MASK_NAMED        2
#define NOB_MASK_HOMOGENEOUS  4
#define NOB_MASK_FAVOR_IBN    8
#define NOB_MASK_GRACEFUL_NSM 16
#define NOB_MASK_IS_NULL      32

NOBcomposite::NOBcomposite(NOBmemoizer* pMemoizer)
 : m_Flags(NOB_MASK_NAMED | NOB_MASK_HOMOGENEOUS),
   m_NodeType(-1),
   m_ProtocolType(-1),
   m_pMemoizer(pMemoizer)
{
   COLASSERT(pMemoizer);
}

NODgrammarType NOBcomposite::grammarType() const{
   return (m_Flags & NOB_MASK_COMPLEX) ? NOD_COMPLEX : NOD_VECTOR;
}

const COLstring& NOBcomposite::protocolName() const{
   return m_pMemoizer->protocol(m_ProtocolType);
}

const COLstring& NOBcomposite::nodeTypeName() const{
   return m_pMemoizer->nodeType(m_ProtocolType, m_NodeType);
}

const COLstring& NOBcomposite::name() const{
   return m_pMemoizer->valueFromId(m_NameId);
}

bool NOBcomposite::isNull() const{
   return m_Flags & NOB_MASK_IS_NULL;
}

bool NOBcomposite::isNamed() const{
   return m_Flags & NOB_MASK_NAMED;
}

bool NOBcomposite::isHomogeneous() const{
   return m_Flags & NOB_MASK_HOMOGENEOUS;
}

bool NOBcomposite::favorIndexByName() const{
   return m_Flags & NOB_MASK_FAVOR_IBN;
}

bool NOBcomposite::gracefulNoSuchMember() const{
   return m_Flags & NOB_MASK_GRACEFUL_NSM;
}

void NOBcomposite::setName(const COLstring& Name){
   m_NameId = m_pMemoizer->idFromValue(Name);
}

NODplace* NOBcomposite::addChild(NODplace* pChild){
   int ChildIndex = size();
   insertChildCache(ChildIndex, pChild);
   return pChild;
}

NODplace* NOBcomposite::initDummyItem(NODplace* pDummy){
   m_pDummyItem = pDummy;
   return pDummy;
}

NODplace* NOBcomposite::newChild(int i, const char* Arg1, const char* Arg2){
   COLASSERT(i == -1);
   COLASSERT(m_pDummyItem.get());
   return m_pDummyItem.get();
}

COLint8 NOBgenerateCompositeFlags(const NODplace& Place){
   return
      (Place.isComplex()                             ? NOB_MASK_COMPLEX      : 0) |
      (Place.isNamed()                               ? NOB_MASK_NAMED        : 0) |
      (Place.isVector() && Place.isHomogeneous()     ? NOB_MASK_HOMOGENEOUS  : 0) |
      (Place.isComplex() && Place.favorIndexByName() ? NOB_MASK_FAVOR_IBN    : 0) |
      (Place.gracefulNoSuchMember()                  ? NOB_MASK_GRACEFUL_NSM : 0) |
      (Place.isNull()                                ? NOB_MASK_IS_NULL      : 0);
}
