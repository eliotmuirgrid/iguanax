// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDluaRegister
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Wed 28 September 2022 - 02:20PM
// ---------------------------------------------------------------------------

#include <DBD/DBDluaRegister.h>
#include <DBD/DBDworld.h>
#include <DOC/DOClapi.h>
#include <LUA/LUAutils.h>
#include <LHL7/LHL7api.h>
#include <LX12/LX12api.h>
#include <NODL/NODLenvironment.h>
#include <WEB/WEBserver.h>
#include <LJSON/LJSONapi.h>
#include <LIV/LIVapi.h>
#include <LUI/LUIapi.h>
#include <LCR/LCRapi.h>
#include <LSFI/LSFItextFilters.h>
#include <LCOM/LCOMcomponent.h>
#include <LUA/LUAmessage.h>
#include <LDB/LDBapi.h>
#include <LHLP/LHLPmapMethods.h>
#include <LHLP/LHLPindex.h> // for LHLPcreateHelpTable
#include <LIG/LIGapi.h>
#include <LSCK/LSCKsocketApi.h>
#include <LQAPI/LQAPIqueue.h>
#include <LUTIL/LUTILapi.h>
#include <LSTR/LSTRstring.h>
#include <LOSE/LOSEapi.h>
#include <LXML/LXMLapi.h>
#include <LNET/LNETnet.h> // for CURL apis
#include <LNET/LNETcurl.h>
#include <LNET/LNETcurlFtp.h>
#include <LTEST/LTEST.h>
#include <LTUN/LTUNapi.h>
#include <LTUN/LTUNconnectionId.h>

#include <LND/LNDcache.h>
#include <LND/LNDapi.h>
#include <LND/LNDutils.h>

#include <LAPI/LAPIapi.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDluaRegister(lua_State* L, LUAmode Mode, DBDworld* pWorld) {
   COL_FUNCTION(DBDluaRegister);
   // TODO UGLY unprequalified names for these constants
   LUAsetPointerField(L, "_pEventLoop",       pWorld->pLoop);
   LUAsetPointerField(L, "_pConfigMap",       pWorld->pComponentMap);
   LUAsetPointerField(L, "_pTunnelState",     pWorld->pTunnelStateClient);
   LUAsetPointerField(L, "_pGridNode",        pWorld->pGridNode);
   LUAsetPointerField(L, "LUAuserMap",        pWorld->pUserMap);
   LUAsetPointerField(L, NODL_DATABASE_FACTORY.c_str(), pWorld->pDatabaseFactory);
   COL_TRC("After set pointers");

   LTUNsetConnectionId(L, pWorld->pLoop); // this gets a connection id from SCKloop and sets it to _pConnectionId
   DOClapi(L);
   LHL7api(L);
   LX12api(L);
   LJSONapiAddJsonNamespace(L);
   LIVaddIconvNamespace(L);
   LCRaddCryptoNamespace(L);
   LSFItextFiltersAddNamespace(L);  // The "filter" Namespace
   LCOMaddComponentNamespace(L, pWorld->pComponentMap, pWorld->pBluaManager, pWorld->pLoop);
   LUAsetupDefaultMessage(L);
   LDBaddDatabaseNamespace(L);
   LDBaddDatabaseConnectionCount(L);
   LHLPcreateHelpTable(L);
   LHLPaddHelpNamespace(L);
   LIGapiAddIguanaNamespace(L);
   LIGapiAddIguana6Namespace(L);
   LSCKaddSocketNamespace(L);
   LQAPIaddQueueNamespace(L, Mode, pWorld->pComponentMap, pWorld->pBluaManager, pWorld->pLoop);
   LUTILapiAddUtilNamespace(L);
   LUInamespace(L);
   // Experimental: Adding to the core string functionality
   // TODO : Include namespacing for nodes with __tostring as well (if desired)
   LSTRaddStringNamespace(L);
	LTESTaddTestNameSpace(L);
   LOSEaddOsExtensions(L);
   LXMLaddXmlNamespace(L);
   LNETaddNetNamespace(L);
   LTUNapi(L);

   LNDaddNodeNamespace(L);
   LNDdefineTree(L);

   LAPIapi(L);
   LAPIremoveUnsupportedFunctions(L);

   NODLcreateDatabaseCache(L);
   LNETcreateCurlCache(L);
   LNETcreateCurlFtpCache(L);
   LUAosSuppress(L);  // suppress some stuff which is a pain in embedded Lua.
}

// TODO - I don't think that the translator IDE calls these shutdown functions IX-1719
// Called on an idle Lua state after a while.  Cleans up database etc.
void DBDluaIdle(lua_State* L){
   COL_FUNCTION(DBDluaIdle);
   LNETcurlInactiveCleanup(L);
   LNETcurlFtpInactiveCleanup(L);
   NODLdbInactiveConnectionCleanup(L);
}
