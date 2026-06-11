#ifndef __DBD_WORLD_H__
#define __DBD_WORLD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDworld
//
// Description:
//
// This is the 'god' object which has a pointer to everything.  It feels
// difficult to avoid such an object when it comes to start up and shut down
// routines.
//
// The danger of this object is that it becomes a way to be sloppy and pass it
// around in many contexts such that couple libraries together and stops us
// from separating concerns.
//
// Author: Eliot Muir
// Date:   Monday 19 September 2022 - 12:57PM
// ---------------------------------------------------------------------------
// These objects cannot be predeclared because they have complicated typedef etc.
#include <GSV/GSVconfig.h>
#include <USR/USRmap.h>
#include <COL/COLarray.h>
#include <COL/COLhashmap.h>
#include <DBD/DBDmode.h>
#include <DRX/DRXlicenseValidator.h>
#include <SDBS/SDBSdetailSearchState.h>
#include <SDBS/SDBSretrieveState.h>

class ACTactionSet;
class BLUAinstanceManager;
class COLclosure0;
class COLthreadPool;
class DBdatabaseFactory;
class GITEbitbucketAuth;
class GRIDnode;
class IDBdatabaseConfig;
class IWEBconfig;
class LOGEuseEncryption;
class NOTImodel;
class PEERmap;
class ROLmap;
class SCKloop;
class SDBsystem;
class SDMserver;
class TUNstateClient;
class WEBserver;
class SDBAresubmitBulkState;
;

struct CFGmap;
struct GINScommitInfo;
struct TRNjobQueue;

// DANGER - DO NOT USE THIS OBJECT OUTSIDE OF DBD.  See above comment
class DBDworld {
  public:
   DBDworld();  // To track lifetime.
   ~DBDworld();

   DBD_MODE Mode = DBD_MODE::INSTALLER;

   BLUAinstanceManager* pBluaManager       = nullptr;
   CFGmap*              pComponentMap      = nullptr;
   DBdatabaseFactory*   pDatabaseFactory   = nullptr;
   DRXlicenseValidator* pLicenseValidator  = nullptr;
   IDBdatabaseConfig*   pDatabaseConfig    = nullptr;
   IWEBconfig*          pWebConfig         = nullptr;
   NOTImodel*           pNotifications     = nullptr;
   PEERmap*             pPeerMap           = nullptr;
   ROLmap*              pRoleMap           = nullptr;
   SDBsystem*           pLogQueue          = nullptr;
   SDMserver*           pServer            = nullptr;
   SCKloop*             pLoop              = nullptr;
   TUNstateClient*      pTunnelStateClient = nullptr;
   WEBserver*           pWebServer         = nullptr;
   GINScommitInfo*      pCommitInfo        = nullptr;
   GRIDnode*            pGridNode          = nullptr;


   COLauto<COLclosure3<COLstring, COLstring, COLclosure1<COLvar>*>> pExtAuthenticator;
   int                                                              ExtAuthTimerId = 0;

   COLthreadPool* pThreadPool = nullptr;

   COLmap<COLstring, USRuser>* pUserMap = nullptr;

   COLhashmap<COLstring, TRNjobQueue*>*         pTranslatorJobQueues = nullptr;
   COLhashmap<COLstring, SDBSretrieveState>*    pLogSearchStateMap   = nullptr;
   COLhashmap<COLstring, SDBSdetailSearchItem>* pLogDetailSearchMap  = nullptr;

   bool StartupFailed = false;  // THIS CAN GO - it's not used InitError serves the same purpose
   bool ReturnFailure = false;  // We use this to signal to return EXIT_FAILURE from DBDrun so that windows services
                                // etc. will restart Iguana.
   bool InitError = false;
   // Prevent multiple git cache refresh operations. This used to also block ssh access checks when they directly used
   // the ssh binary. Now Iguana relies on git to do that so it is okay to have an ssh access check concurrently running
   // with other git operations.
   bool IsGitCacheRefreshing = false;
   bool UseEncryptedLogs     = false;

   bool FreshInstall = false;

   COLstring ErrorLog;
};

#endif  // end of defensive include
