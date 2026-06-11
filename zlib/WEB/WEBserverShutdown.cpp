//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBserverShutdown
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include <WEB/WEBserver.h>
#include <SCK/SCKconnectionWrite.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// See IE-2070 - very important to shut all the sockets connections so the port doesn't stay open.
void WEBserver::shutdown(){
   COL_METHOD(WEBserver::shutdown);
   COL_VAR(m_Connections.size());

   if (m_pListener){
      m_pListener->close();
   }
   COL_VAR2(m_ConnectionTimerId, m_SessionTimerId);
   if (m_ConnectionTimerId){ m_pLoop->cancelTimer(m_ConnectionTimerId); COL_TRC("Stopped connection purge timer.");  m_ConnectionTimerId = 0;}
   if (m_SessionTimerId)   { m_pLoop->cancelTimer(m_SessionTimerId);    COL_TRC("Stopped session timer.");           m_SessionTimerId = 0;   }
   COLlist<COLuint64> CloseList;
   for (auto i=m_Connections.begin(); i != m_Connections.end(); i++){
      CloseList.push_back(i->first);
   }
   for (auto i=CloseList.begin(); i != CloseList.end(); i++){
      COL_VAR(*i);
      SCKconnectionHardClose(m_pLoop, *i);
   }
   COL_VAR2(m_Connections.size(), m_Connections.empty());
   m_pListener = NULL;
}