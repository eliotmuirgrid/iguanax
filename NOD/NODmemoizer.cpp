//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODmemoizer
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Monday, September 19th, 2011 @ 10:57:39 AM
//
//---------------------------------------------------------------------------
#include "NODprecomp.h"
#pragma hdrstop

#include "NODmemoizer.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLint64 NODmemoizer::idFromValue(const COLstring& Value){
   COLhashmapPlace Place = m_Lookup.find(Value);
   if (!Place){
      Place = m_Lookup.add(Value, m_NextId);
      m_ReverseLookup.add(m_NextId, &m_Lookup.key(Place));
      m_NextId++;
      if (m_NextId == COL_INT_64_MAX){
         COL_ERROR_STREAM("Too many string values to memoize.", COLerror::PreCondition);
      }
   }
   return m_Lookup[Place];
}

const COLstring& NODmemoizer::valueFromId(const COLint64 Id) const{
   return *m_ReverseLookup[Id];
}

void NODmemoizer::add(const COLint64 Id, const COLstring& Value){
   COLhashmapPlace Place = m_Lookup.add(Value, Id);
   m_ReverseLookup.add(Id, &m_Lookup.key(Place));
   m_NextId = m_Lookup.size();
}
