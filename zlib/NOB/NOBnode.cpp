//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBnode
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Wednesday, November 3rd, 2010 @ 11:44:42 AM
//
//---------------------------------------------------------------------------
#include "NOBprecomp.h"
#pragma hdrstop

#include "NOBnode.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

NOBnode* NOBnode::addChild(){
   int ChildIndex = size();
   insertChildCache(ChildIndex);
   return (NOBnode*)&child(ChildIndex);
}

NOBnode* NOBnode::initDummyItem(){
   m_pDummyItem = new NOBnode();
   return m_pDummyItem.get();
}

NODplace* NOBnode::newChild(int i, const char* Arg1, const char* Arg2){
   COLASSERT(i == -1);
   COLASSERT(m_pDummyItem.get());
   return m_pDummyItem.get();
}
