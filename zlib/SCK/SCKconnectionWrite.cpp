// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKconnectionWrite
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 20 June 2023 - 01:51PM
// ---------------------------------------------------------------------------

#include <SCK/SCKconnectionWrite.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKconnection.h>
#include <SCK/SCKwrite.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


void SCKconnectionWrite(SCKloop* pLoop, COLuint64 ConnectionId, const char* pBuffer, int Size){
   COL_FUNCTION(SCKconnectionWrite);
   COL_VAR(pLoop);
   auto i = pLoop->m_ConnectionMap.find(ConnectionId);
   if (!i){
      COL_ERR("Connection " << ConnectionId << " has closed.");
      return;
   }
   i->second->write(pBuffer, Size);
}


void SCKconnectionClose(SCKloop* pLoop, COLuint64 ConnectionId){
   COL_FUNCTION(SCKconnectionClose);
   auto i = pLoop->m_ConnectionMap.find(ConnectionId);
   if (!i){
      COL_ERR("Connection " << ConnectionId << " has closed already.");
      return;
   }
   i->second->close();
   i->second->checkPendingClose();
}


void SCKconnectionHardClose(SCKloop* pLoop, COLuint64 ConnectionId){
   COL_FUNCTION(SCKconnectionHardClose);
   auto i = pLoop->m_ConnectionMap.find(ConnectionId);
   if (!i){
      COL_ERR("Connection " << ConnectionId << " has closed already.");
      return;
   }
   i->second->hardClose();
}