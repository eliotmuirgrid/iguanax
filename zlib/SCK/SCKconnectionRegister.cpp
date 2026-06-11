// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKconnectionRegister
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 20 June 2023 - 10:47AM
// ---------------------------------------------------------------------------

#include <SCK/SCKconnectionRegister.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKconnection.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


void SCKconnectionRegister(SCKloop* pLoop, SCKconnection* pConnection){
   COL_FUNCTION(SCKconnectionRegister);
   pLoop->m_NextSocketId++;
   pLoop->m_ConnectionMap.add(pLoop->m_NextSocketId, pConnection);
   pConnection->m_Id = pLoop->m_NextSocketId;
   COL_VAR2(pLoop->m_NextSocketId, pLoop->m_ConnectionMap.size());
}

void SCKconnectionUnregister(SCKloop* pLoop, COLuint64 Id){
   COL_FUNCTION(SCKconnectionUnregister);
   pLoop->m_ConnectionMap.removeKey(Id);
   COL_VAR2(Id, pLoop->m_ConnectionMap.size());
}