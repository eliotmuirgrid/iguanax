//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NOBmemoizer
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, September 15th, 2011 @ 02:16:20 PM
//
//---------------------------------------------------------------------------
#include "NOBprecomp.h"
#pragma hdrstop

#include "NOBmemoizer.h"
#include "NOBcomposite.h"
#include "NOBsimpleEmpty.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

void NOBmemoizer::addProtocol(int ProtocolId, const COLstring& ProtocolName){
   if (!m_ProtocolLookup.find(ProtocolId)){
      m_ProtocolLookup.add(ProtocolId, ProtocolName);
   }
}

static int NOBnodeTypeKey(int ProtocolId, int NodeTypeId){
   return ProtocolId*10000 + NodeTypeId;
}

void NOBmemoizer::addNodeType(int ProtocolId, int NodeTypeId, const COLstring& NodeTypeName){
   const int Key = NOBnodeTypeKey(ProtocolId, NodeTypeId);
   if (!m_NodeTypeLookup.find(Key)){
      m_NodeTypeLookup.add(Key, NodeTypeName);
   }
}

const COLstring& NOBmemoizer::protocol(int ProtocolId) const{
   return m_ProtocolLookup[ProtocolId];
}

const COLstring& NOBmemoizer::nodeType(int ProtocolId, int NodeTypeId) const{
   return m_NodeTypeLookup[NOBnodeTypeKey(ProtocolId, NodeTypeId)];
}

void NOBmemoizer::addRawNodeType(int Key, const COLstring& NodeTypeName){
   m_NodeTypeLookup.add(Key, NodeTypeName);
}

const NOBmemoizer* NOBgetMemoizer(const NODplace& NobNode){
   if (const NOBcomposite* pComposite = dynamic_cast<const NOBcomposite*>(&NobNode)){
      return pComposite->memoizer();
   } else if (const NOBsimpleEmpty* pSimple = dynamic_cast<const NOBsimpleEmpty*>(&NobNode)){
      return pSimple->memoizer();
   } else {
      return NULL;
   }
}
