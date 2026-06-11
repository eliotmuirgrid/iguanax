//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBserverStart
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include <WEB/WEBserver.h>
#include <WEB/WEBserverPurge.h>
#include <WEB/WEBsession.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

bool WEBserver::start(int Port, const COLstring& BindIp, bool IsSecure){
   COL_METHOD(WEBserver::start);
   m_ConnectionTimerId = WEBpurgeTimerRun(m_pLoop, &m_Connections);
   m_SessionTimerId = m_pLoop->addTimer(COLnewClosure0(&WEBsessionRemoveOld, &m_SessionMap, 60), 10000, true);
   COL_VAR3(Port, BindIp, IsSecure);
   if (IsSecure){
      m_pCtx = &m_SslContext;
   } else {
      m_pCtx = NULL;
   }
   m_pListener = new SCKlistener(*m_pLoop);
   m_pListener->setOnAccept(COLnewClosure0(this, &WEBserver::onAccept));
   bool Success = m_pListener->listen(Port, BindIp);
   if (!Success){
      shutdown();
   }
   return Success;
}
