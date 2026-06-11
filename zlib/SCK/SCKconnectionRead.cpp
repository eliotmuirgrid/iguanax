// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKconnectionRead
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 20 June 2023 - 02:13PM
// ---------------------------------------------------------------------------

#include <SCK/SCKconnectionRead.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKconnection.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLstring Empty;

void SCKconnectionRead(SCKloop* pLoop, COLuint64 ConnectionId, COLstring** ppInputBuffer){
   COL_FUNCTION(SCKconnectionRead);
   auto i = pLoop->m_ConnectionMap.find(ConnectionId);
   if (!i){
      COL_ERR("Connection has gone away.");
      *ppInputBuffer = &Empty;
      return;
   }
   *ppInputBuffer = &i->second->m_BufferIn;
}