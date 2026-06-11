//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODtype
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Friday, December 23rd, 2011 @ 04:39:26 PM
//
//---------------------------------------------------------------------------
#include "NODprecomp.h"
#pragma hdrstop

#include "NODtype.h"

#include <COL/COLhashmap.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

class NODprotocolAndNodeTypePair{
public:
   NODprotocolAndNodeTypePair(int Protocol, int NodeType) : m_Protocol(Protocol), m_NodeType(NodeType) {}
   int m_Protocol;
   int m_NodeType;
};

static COLhashmap<COLstring, NODprotocolAndNodeTypePair> NODtypeMap;

void NODtypeMapAdd(const COLstring& NodeTypeName, int NodeType, const COLstring& ProtocolName, int Protocol){
   COL_FUNCTION(NODtypeMapAdd);
   NODprotocolAndNodeTypePair Pair(Protocol, NodeType);
   NODtypeMap.add(ProtocolName + ":" + NodeTypeName, Pair);
}

void NODprotocolAndNodeTypeNameToIds(const COLstring& CombinedName, int& Protocol, int& NodeType){
   const COLhashmap<COLstring, NODprotocolAndNodeTypePair>& Map = NODtypeMap;
   COLhashmapPlace Place = Map.find(CombinedName);
   if (Place){
      const NODprotocolAndNodeTypePair& Pair = Map[Place];
      Protocol = Pair.m_Protocol;
      NodeType = Pair.m_NodeType;
   } else {
      Protocol = -1;
      NodeType = -1;
   }
}
