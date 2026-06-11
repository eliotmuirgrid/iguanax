// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKwrite
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 19 June 2023 - 04:48PM
// ---------------------------------------------------------------------------

#include <SCK/SCKwrite.h>
#include <SCK/SCKconnection.h>
#include <SCK/SCKtruncate.h>
#include <SCK/SCKerror.h>
#include <SCK/SCKloop.h>
#include <SCK/SCKonWritable.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int SCKwrite(SCKconnection* pConnection){
   COL_FUNCTION(SCKwrite);
   int BytesSent = ::send(pConnection->m_Handle, pConnection->m_BufferOut.c_str(), pConnection->m_BufferOut.size(), 0);
   COL_VAR(BytesSent);
   if (BytesSent == -1){
      pConnection->recordLastError();  // Must do this before we do any tracing.
      if (pConnection->m_LastError == SCK_EAGAIN){
         COL_TRC("EWOULDBLOCK so need to ask for when we can write again.  Setting the bytes sent to 0 since it's not a real error.");
         pConnection->m_pLoop->addWriteCallback(pConnection->m_Handle);
         pConnection->m_Writable = false;
         return -1;
      } else {
         SCKshutdown(pConnection);
         return -1;
      }
      return -1;
   }
   pConnection->m_TotalWritten += BytesSent;
   COL_VAR2(BytesSent, pConnection->m_TotalWritten);
   SCKtruncateBuffer(BytesSent, &pConnection->m_BufferOut);
   if (pConnection->m_BufferOut.size() > 0){
      COL_TRC("Socket output buffer is full so we have to ask for when we can write again.");
      pConnection->m_pLoop->addWriteCallback(pConnection->m_Handle);
      COL_VAR(pConnection->m_BufferOut.size());
   } else {
      COL_TRC("Clear write notifications.");
      pConnection->m_pLoop->removeWriteCallback(pConnection->m_Handle);
      SCKonWritable(pConnection);
   }
   pConnection->checkPendingClose();
   return BytesSent;
}
