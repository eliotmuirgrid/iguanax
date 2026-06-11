// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDsetup
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday 19 September 2022 - 02:04PM
// ---------------------------------------------------------------------------
#include <BLUA/BLUAmanager.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <DB/DBdatabaseFactory.h>
#include <DBD/DBDextAuth.h>
#include <DBD/DBDsession.h>
#include <DBD/DBDsetup.h>
#include <DBD/DBDusage.h>
#include <DBD/DBDworld.h>
#include <DIR/DIRpath.h>
#include <ENV/ENVstartup.h>
#include <GINS/GINScommitInfo.h>
#include <GITE/GITEbitbucket.h>
#include <GRID/GRIDnode.h>
#include <IDB/IDBdatabaseConfig.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEBsetup.h>
#include <LOGE/LOGEencrypt.h>
#include <NOTI/NOTImodel.h>
#include <PEER/PEERmap.h>
#include <ROL/ROLrole.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBserviceLog.h>
#include <SDB/SDBsystem.h>
#include <SDBU/SDBUencryptionKey.h>
#include <TRN/TRNjobQueue.h>
#include <TRN/TRNrequestCheckValid.h>
#include <TUN/TUNstateClient.h>
#include <USR/USRsetup.h>
#include <WEB/WEBconnectionCheck.h>
#include <WEB/WEBserver.h>
COL_LOG_MODULE;

void DBDcreateCore(DBDworld* pWorld, const COLstring& LogEncryptionKey) {
   COL_FUNCTION(DBDcreateCore);
   IWEBcreate(&pWorld->pWebConfig);
   pWorld->pThreadPool = new COLthreadPool(1, 1000);
   pWorld->pThreadPool->start();
   ENVstartup();
   SDBstartServiceLog(pWorld->pThreadPool, LogEncryptionKey);
   pWorld->pLoop       = new SCKloop(pWorld->pThreadPool);
   pWorld->pWebServer  = new WEBserver(*pWorld->pLoop);
   pWorld->pUserMap    = USRcreate();
   pWorld->pRoleMap    = new ROLmap();
}

void DBDcreate(DBDworld* pWorld, const COLstring& LogEncryptionKey) {
   COL_FUNCTION(DBDcreate);
   DBDcreateCore(pWorld);
   SDBUsetEncryptionKeyGlobal(LogEncryptionKey);
   pWorld->pCommitInfo = new GINScommitInfo();
   pWorld->UseEncryptedLogs = !LogEncryptionKey.is_null();
   WEBsetConnectionCheckClosure(pTRNrequestCheckValid, pWorld->pWebServer);
   DBDcreateSession(pWorld->pUserMap, pWorld->pWebServer);  // TODO move me - this function just sets callbacks.
   pWorld->pComponentMap        = new CFGmap();
   pWorld->pNotifications       = new NOTImodel(pWorld->pLoop);
   pWorld->pGridNode            = nullptr;  // Initialized later in DBDgridInit
   pWorld->pLogQueue            = new SDBsystem(pWorld->pLoop, *pWorld->pComponentMap, LogEncryptionKey);
   pWorld->pBluaManager         = new BLUAinstanceManager();
   pWorld->pDatabaseFactory     = new DBdatabaseFactory();
   pWorld->pDatabaseConfig      = new IDBdatabaseConfig();
   pWorld->pLicenseValidator    = new DRXlicenseValidator(pWorld->pLoop);
   pWorld->pTranslatorJobQueues = new COLhashmap<COLstring, TRNjobQueue*>();
   pWorld->pLogSearchStateMap   = new COLhashmap<COLstring, SDBSretrieveState>();
   pWorld->pLogDetailSearchMap  = new COLhashmap<COLstring, SDBSdetailSearchItem>();
   pWorld->pPeerMap             = new PEERmap();
   pWorld->pTunnelStateClient   = new TUNstateClient();
   pWorld->pExtAuthenticator    = COLnewClosure3(&DBDextAuthenticateUser, pWorld->pBluaManager, pWorld->pComponentMap, pWorld->pLoop);
   CFGmapLookupSet(pWorld->pComponentMap);
   DBDextAuthSetupRefresh(pWorld->pBluaManager, pWorld->pComponentMap, pWorld->pUserMap,
                          &pWorld->pWebServer->m_SessionMap, pWorld->pLoop, &pWorld->ExtAuthTimerId);
}

void DBDdestroy(DBDworld* pWorld) {
   COL_FUNCTION(DBDdestroy);
   COLcout << "Waiting for threads to complete..." << newline;
   SDBstopServiceLog();
   delete pWorld->pThreadPool;                             COL_TRC("Destroyed COLthreadPool");
   pWorld->pWebServer->shutdown();
   DBDextAuthTearDownRefresh(pWorld->pLoop, pWorld->ExtAuthTimerId);
   delete pWorld->pWebServer;                              COL_TRC("Destroyed Web Server");
   WEBdeleteConnectionCheckClosure(pTRNrequestCheckValid); COL_TRC("Destroyed connection check closure");
   IWEBdestroy(&pWorld->pWebConfig);                       COL_TRC("Destroyed web server configuration.");
   DBDstopUsageCollection();
   delete pWorld->pGridNode;                               COL_TRC("Destroyed GRID node.");
   delete pWorld->pCommitInfo;                             COL_TRC("Destroyed commit info struct.");
   delete pWorld->pComponentMap;                           COL_TRC("Destroyed component map.");
   delete pWorld->pLicenseValidator;                       COL_TRC("Destroyed license validator");
   delete pWorld->pNotifications;                          COL_TRC("Destroyed notifications.");
   delete pWorld->pBluaManager;                            COL_TRC("Destroyed back end lua instances.");
   delete pWorld->pDatabaseFactory;                        COL_TRC("Destroyed database factory.");
   delete pWorld->pDatabaseConfig;                         COL_TRC("Destroyed database config.");
   delete pWorld->pTranslatorJobQueues;                    COL_TRC("Destroyed hashmap of job queues.");
   delete pWorld->pLogSearchStateMap;                      COL_TRC("Destroyed hashmap of log search states.");
   delete pWorld->pLogDetailSearchMap;                     COL_TRC("Destroyed hashmap of log detail search items.");
   delete pWorld->pPeerMap;                                COL_TRC("Destroyed hashmap of peers");
   delete pWorld->pUserMap;                                COL_TRC("Destroyed map of users.");
   delete pWorld->pRoleMap;                                COL_TRC("Destroyed map of user roles.");
   delete pWorld->pTunnelStateClient;                      COL_TRC("Destroyed the tunnel client state.");
   delete pWorld->pLogQueue;                               COL_TRC("Destroyed log system");
   delete pWorld->pLoop;                                   COL_TRC("Destroyed SCKloop");
}
