// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBserverDispatch
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday 22 May 2023 - 03:22PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <WEB/WEBfileHandler.h>
#include <WEB/WEBheartbeat.h>
#include <WEB/WEBrequestData.h>
#include <WEB/WEBresponse.h>
#include <WEB/WEBserver.h>
#include <WEB/WEBserverDispatch.h>
#include <WEB/WEBserverDispatchWorker.h>
#include <WEB/WEBsessionHandler.h>
COL_LOG_MODULE;

void WEBserverDispatch(const WEBrequestData& RequestData,
    COLmap<COLstring, WEBsession>* pSessionMap,
    SCKloop* pLoop,
    bool RequireAuthentication,
    COLhashmap<COLstring, COLwebHandlerFunc>& WorkerThreadHandlers,
    COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& MainThreadHandlers,
    WEBserver* pServer)
{
   COL_FUNCTION(WEBserverDispatch);
   try{
      COL_TRC(RequestData.Path);
      if(RequestData.Path == "/session/heartbeat") {
         WEBheartbeat(RequestData, pSessionMap);  // Special case - we need to access the session cookie
         return;
      }
      if(RequestData.Path == "/saml/verify" && pServer->m_pSamlPostHandler.get()) {
         COL_TRC("Receieved SAML post");
         return pServer->m_pSamlPostHandler->run(RequestData);
      }
      // bool WillSpecialHandle = false;
      // if (pServer->m_pWillSpecialHandle.get()){
      //    pServer->m_pWillSpecialHandle->run(RequestData, &WillSpecialHandle);
      // }

      if (WorkerThreadHandlers.count(RequestData.Path) + MainThreadHandlers.count(RequestData.Path) > 0){
         COLstring User;
         bool AdminUser = false;
         if (!RequireAuthentication){
            COL_TRC("No authentication required.  We are in startup mode.");
         } else {
            if (!WEBsessionAuthenticate(RequestData, *pSessionMap, &User, &AdminUser)){
               if (!WEBsessionNologin(RequestData.Path)){
                  WEBresponseError(RequestData.ConnectionId, "Not authenticated");
                  return;
               }
               COL_TRC("This call does not require a login.");
            }
         }
         // if (WillSpecialHandle){
         //    pServer->m_pSpecialHandler->run(RequestData);
         //    return;
         // }
         if (WorkerThreadHandlers.count(RequestData.Path)){
            COL_TRC("Request handled server dispatch.");
            WEBserverDispatchWorker(RequestData, WorkerThreadHandlers, User, AdminUser, pLoop);
            return;
         }
         if (MainThreadHandlers.count(RequestData.Path)){
            COL_TRC("Handling main event dispatch");
            WEBserverDispatchMain(RequestData, MainThreadHandlers, User, AdminUser);
            return;
         }
      } else {
         COL_TRC("No handler existed for " << RequestData.Path);
      }
      WEBfileResponse(RequestData, pLoop, pServer->m_IndexPage);
   } catch(const COLerror& Error){
      WEBresponseError(RequestData.ConnectionId, Error.description());
   }
}
