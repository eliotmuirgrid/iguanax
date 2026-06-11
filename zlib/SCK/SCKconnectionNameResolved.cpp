// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKconnectionNameResolved
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Tuesday 20 June 2023 - 11:03AM
// ---------------------------------------------------------------------------

#include <SCK/SCKconnectionNameResolved.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKconnection.h>
#include <SCK/SCKaddress.h>
#include <SCK/SCKerror.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SCKconnectionNameResolved(const SCKaddress& Address, SCKloop* pLoop, COLuint64 ConnectionId){
   COL_FUNCTION(SCKconnectionNameResolved);
   auto i = pLoop->m_ConnectionMap.find(ConnectionId);
   if (!i){
      COL_TRC("Connection has been terminated already.");
   } else {
      if (!Address.set()){
         COL_TRC("Could not resolve hostname.");
         i->second->m_LastError = SCK_DNSFAILURE;
         i->second->hardClose();
         return;
      } else {
         COL_TRC("Commence with connect to " << Address);
         i->second->connect(Address);
      }
   }
}
