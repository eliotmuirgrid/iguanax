// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday 19 September 2022 - 02:20PM
// ---------------------------------------------------------------------------
#include "DBDapi.h"

#include <GRIDA/GRIDAapi.h>
#include <BLUA/BLUAapi.h>
#include <CMP/CMPapi.h>
#include <CMPA/CMPAapi.h>
#include <COL/COLlog.h>
#include <DASH/DASHapi.h>
#include <DBD/DBDmodeApi.h>
#include <DBD/DBDserverApi.h>
#include <DBD/DBDworld.h>
#include <DBDUP/DBDUPapi.h>
#include <DBDVER/DBDVERapi.h>
#include <DBG/DBGapi.h>
#include <DBIM/DBIMapi.h>
#include <DBS/DBSapi.h>
#include <DEP/DEPapi.h>
#include <DIR/DIRpath.h>
#include <DOC/DOCapi.h>
#include <DRX/DRXlicenseApi.h>
#include <DTL/DTLapi.h>
#include <EDIC/EDICapi.h>
#include <ENV/ENVapi.h>
#include <EXPT/EXPTapi.h>
#include <FSB/FSBapi.h>
#include <GINI/GINIapi.h>
#include <GINR/GINRapi.h>
#include <GINS/GINSapi.h>
#include <GIT/GITapi.h>
#include <GITC/GITCapi.h>
#include <GITS/GITSapi.h>
#include <GITW/GITWapi.h>
#include <HA/HAapi.h>
#include <HELP/HELPapi.h>
#include <HTOOL/HTOOLapi.h>
#include <ICOL/ICOLapi.h>
#include <IDB/IDBapi.h>
#include <INFO/INFOapi.h>
#include <INST/INSTapi.h>
#include <IWEBA/IWEBapi.h>
#include <LIB/LIBapi.h>
#include <LOGE/LOGEapi.h>
#include <LOGG/LOGGapi.h>
#include <LOGR/LOGRapi.h>
#include <LOGV/LOGVapi.h>
#include <NOTI/NOTIapi.h>
#include <PEER/PEERapi.h>
#include <PRM/PRMapi.h>
#include <ROL/ROLapi.h>
#include <SCK/SCKcall.h>
#include <SDBA/SDBAapi.h>
#include <SDBS/SDBSapi.h>
#include <SEC/SECapi.h>
#include <SRCH/SRCHapi.h>
#include <SSO/SSOapi.h>
#include <TPJ/TPJapi.h>
#include <TRN/TRNapi.h>
#include <TRNC/TRNCapi.h>
#include <TUN/TUNapi.h>
#include <USR/USRapi.h>
#include <WEB/WEBapi.h>
#include <WEB/WEBserver.h>
COL_LOG_MODULE;

// TODO - better names for event loop versus threaded maps.
static void DBDregisterApi(COLhashmap<COLstring, COLwebHandlerFunc>& WorkerThreadHandlers,
                           COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& NewSyncMap,
                           DBDworld* pWorld) {
   COL_FUNCTION(DBDregisterApi);
   BLUAapi        (NewSyncMap, pWorld->pLoop, pWorld->pComponentMap, pWorld->pBluaManager);
   DASHapi        (NewSyncMap, pWorld->pLoop, pWorld->pComponentMap);
   CMPAapiMain    (NewSyncMap, pWorld->pLoop, pWorld->pWebConfig, pWorld->pBluaManager, pWorld->pComponentMap, &pWorld->IsGitCacheRefreshing);
   DBDserverApi   (NewSyncMap, pWorld->pWebConfig);
   DRXlicenseApi  (NewSyncMap, pWorld->pLoop, pWorld->pLicenseValidator);
   DBDVERapi      (NewSyncMap);
   DBIMapi        (NewSyncMap, pWorld->pLoop, pWorld->pDatabaseFactory);
   DBSapi         (NewSyncMap, pWorld->pComponentMap, pWorld->pLoop);
   DOCapiA        (NewSyncMap, pWorld->pBluaManager);
   DTLapi         (NewSyncMap, pWorld->pComponentMap, pWorld->pLoop);
   GINRapi        (NewSyncMap);
   GINSapi        (NewSyncMap, pWorld->pLoop, pWorld->pUserMap, pWorld->pCommitInfo);
   GITWapi        (NewSyncMap,&pWorld->IsGitCacheRefreshing, pWorld->pLoop);
   GITCmain       (NewSyncMap,&pWorld->IsGitCacheRefreshing, pWorld->pUserMap, pWorld->pLoop);
   GITapi         (NewSyncMap, pWorld->pLoop, pWorld->pComponentMap);
   GRIDAapi       (NewSyncMap, pWorld->pLoop, pWorld->pGridNode, pWorld->pComponentMap);
   ICOLapi        (NewSyncMap);
   IDBapi         (NewSyncMap, pWorld->pLoop, pWorld->pDatabaseConfig);
   INFOapi        (NewSyncMap);
   INSTapiReplace (NewSyncMap, pWorld->pLoop, pWorld);
   IWEBapi        (NewSyncMap, pWorld->pLoop, pWorld->pWebConfig, pWorld->pWebServer);
   LIBapi         (NewSyncMap, pWorld->pComponentMap, pWorld->pLoop);
   LOGEapiMain    (NewSyncMap, &pWorld->UseEncryptedLogs, pWorld->pBluaManager, pWorld->pLoop);
   LOGGeneratorApi(NewSyncMap, pWorld->pLogQueue, pWorld->pLoop);
   NOTIapi        (NewSyncMap, pWorld->pLoop, pWorld->pComponentMap, pWorld->pBluaManager, pWorld->pNotifications);
   PEERapi        (NewSyncMap, pWorld->pPeerMap, pWorld->pWebConfig, pWorld->pLoop);
   PRMapi         (NewSyncMap, pWorld->pLoop, pWorld->pComponentMap);
   ROLapi         (NewSyncMap, pWorld->pLoop, pWorld->pRoleMap);
   SDBAblua       (NewSyncMap, pWorld->pLoop, pWorld->pComponentMap, pWorld->pBluaManager, pWorld->pLogQueue);
   SDBAlog        (NewSyncMap, pWorld->pLogQueue, pWorld->pLoop);
   SDBSlogSearch  (NewSyncMap, pWorld->pLogSearchStateMap, pWorld->pLoop);
   SDBSlogDetailSearch (NewSyncMap, pWorld->pLogDetailSearchMap, pWorld->pLoop);
   SSOapi         (NewSyncMap, pWorld->pUserMap, &pWorld->pWebServer->m_SessionMap,pWorld->pLoop);
   TRNCapi        (NewSyncMap, pWorld->pLoop, pWorld->pComponentMap);
   TRNapi         (NewSyncMap, pWorld->pLoop, pWorld->pTranslatorJobQueues);
   USRapi         (NewSyncMap, pWorld->pLoop, pWorld->pUserMap);
   DBDmodeApi     (NewSyncMap, pWorld);
   WEBapi         (NewSyncMap,&pWorld->pWebServer->m_SessionMap, pWorld->pUserMap, pWorld->pExtAuthenticator); // handles login/logout etc.
   TUNapi         (NewSyncMap, pWorld->pTunnelStateClient);
   SECapi         (NewSyncMap, pWorld->pLoop);
   CMPapi         (WorkerThreadHandlers);
   DBDUPapi       (WorkerThreadHandlers);
   DBGapiAsync    (WorkerThreadHandlers);
   DOCapi         (WorkerThreadHandlers);
   DTLapiAsync    (WorkerThreadHandlers);
   EDICapi        (WorkerThreadHandlers);
   ENVapiAsync    (WorkerThreadHandlers);
   EXPTapi        (WorkerThreadHandlers);
   FSBapi         (WorkerThreadHandlers);
   GINIapi        (WorkerThreadHandlers);
   GITapiAsync    (WorkerThreadHandlers);
   GITCworker     (WorkerThreadHandlers);
   GITSapi        (WorkerThreadHandlers);
   GITWapiAsync   (WorkerThreadHandlers);
   // HAapi          (WorkerThreadHandlers); // IX-4335 disabling for release
   //GRIDapiAsync    (AsyncMap);
   DEPapiWorker   (WorkerThreadHandlers);
   HELPapiWorker  (WorkerThreadHandlers);
   HTOOLapiAsync  (WorkerThreadHandlers);
   INSTapiNormal  (WorkerThreadHandlers);
   LOGRapi        (WorkerThreadHandlers);
   SDBAworker     (WorkerThreadHandlers);
   SRCHapi        (WorkerThreadHandlers);
   TPJapi         (WorkerThreadHandlers);
   TRNCworker     (WorkerThreadHandlers);
   TRNworker      (WorkerThreadHandlers);
   LOGVapi        (WorkerThreadHandlers);
}

void DBDapi(DBDworld* pWorld) {
   COL_FUNCTION(DBDapi);
   // TODO - do we really need to clear these?
   WEBserverClearHandlers(pWorld->pWebServer);
   //pWorld->pGridManager->apiMap().clear();
   DBDregisterApi    (pWorld->pWebServer->m_WorkerThreadHandlers, pWorld->pWebServer->m_MainThreadHandlers, pWorld);
   //DBDregisterApi(pWorld->pGridManager->apiMap(), pWorld->pGridManager->asyncHandlerApiMap(), pWorld);
   SCKdefineSingleton(pWorld->pLoop, &pWorld->pWebServer->m_WorkerThreadHandlers, &pWorld->pWebServer->m_MainThreadHandlers);
}
