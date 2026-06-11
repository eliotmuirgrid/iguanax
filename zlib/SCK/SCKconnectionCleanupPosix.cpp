// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKconnectionCleanupPosix
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 22 June 2023 - 05:34PM
// ---------------------------------------------------------------------------

#include <SCK/SCKconnection.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKconnectionRegister.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SCKconnection::cleanUp(){
   COL_METHOD(SCKconnection::cleanUp);
   COL_VAR2(m_Id, m_Handle);
   if (m_Handle != SCK_INVALID_SOCKET){
      COL_TRC("Closing socket handle.");
      if (m_HardClose){
         COL_TRC("Initiating hard close");
         struct linger so_linger = { true, 0 };
         setsockopt(m_Handle, SOL_SOCKET, SO_LINGER, &so_linger, sizeof(so_linger));
      }
      ::closesocket(m_Handle);
      COL_TRC("Done!");
      m_pLoop->removeAllCallback(m_Handle);
      m_Handle = SCK_INVALID_SOCKET;
   }
   COL_TRC("About to delete the socket.");
   SCKconnectionUnregister(m_pLoop, m_Id);
   delete this;
   COL_TRC("Before return");
}
