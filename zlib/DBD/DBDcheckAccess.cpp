#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <DBD/DBDcheckAccess.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <SCK/SCKloop.h>
#include <SCKB/SCKBconnection.h>
#include <TUN/TUNstateClient.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
COL_LOG_MODULE;

static COLstring DBDerror()          { return "IguanaX is very much in active development.  We regret that to be part of the early\n"
                                               "access program it is necessary for Iguana to have access to the following ports via TCP/IP:"; }
static COLstring DBDbitbucketError() { return "\n  https://bitbucket.org SSH/TCP/IP: This is needed so that Iguana can load components from the repos we host here."; }
static COLstring DBDdownloadsError() { return "\n  https://downloads.interfaceware.com: This is needed to check for the availability of updates and to receive them."; }
static COLstring DBDcrashError()     { return "\n  https://crashreport.interfaceware.com: This is critical for giving information on crash reports so our development team can resolve them."; }
static COLstring DBDtunnelError()    { return "\n  https://tunnel.interfaceware.com: This is important part of the future architecture of Iguana allowing cluster management."; }

static void DBDcreateAccessList(COLmap<COLstring, COLstring>* pAccessMap) {
   COL_FUNCTION(DBDcreateAccessList);
   pAccessMap->add("bitbucket.org:22", DBDbitbucketError());
   pAccessMap->add("https://bitbucket.org:443", DBDbitbucketError());
   pAccessMap->add("https://downloads.interfaceware.com:443", DBDdownloadsError());
   pAccessMap->add("https://crashreport.interfaceware.com:7888", DBDcrashError());
}

class DBDpingTracker {
 public:
   int  Servers    = 0;
   COLstring Error = DBDerror();
   COLmap<COLstring, COLstring> AccessMap;
   SCKloop* pLoop;
};

static void DBDcheckAccessFinished(bool HasAccess, DBDpingTracker* pTracker) {
   COL_FUNCTION(DBDcheckAccessFinished);
   if(!HasAccess) {
      COLcout << pTracker->Error << newline;
      return;
   }
   try {
      FILwriteFile(DBDaccessFile(), "");
   } catch(const COLerror& Error) {
      COL_ERR(Error.description());
   }
}

static void DBDpingCallback(bool Success, COLstring Url, DBDpingTracker* pTracker, bool* pHasAllAccess) {
   COL_FUNCTION(DBDpingCallback);
   pTracker->Servers--;
   COL_VAR(pTracker->Servers);
   if(!Success) {
      COL_TRC(Url << " ping failed" << newline);
      *pHasAllAccess = false;
      pTracker->Error.append(pTracker->AccessMap[Url]);
   }
   if(pTracker->Servers == 0) {
      DBDcheckAccessFinished(*pHasAllAccess, pTracker);
      pTracker->pLoop->shutdown();
      delete pTracker;
   }
}

static void DBDpingServer(COLstring Url, DBDpingTracker* pTracker, bool* pHasAllAccess) {
   COL_FUNCTION(DBDpingServer);
   bool Connected;
   COLstring Host, PortString;
   Url.rsplit(Host, PortString, ":");
   int Port = atoi(PortString.c_str());
   COL_VAR3(Host, PortString, Port);
   if(Port == 22) { // TODO not the best way to check
      COL_TRC("Testing " << Url);
      auto pConnection = new SCKBconnection();
      pConnection->setTimeout(3000);
      Connected = pConnection->connect(Host, Port);
      if(Connected) {
         pConnection->close();
         delete pConnection;
      }
   } else {
      COL_TRC("Testing https access to " << Url);
      WEBresponse Response;
      WEBrequest Request;
      Request.Url = Url;
      WEBrequestBlockingGet(Request, &Response);
      COL_VAR2(Url, Response.ReturnCode);
      Connected = Response.ReturnCode == 200;
   }
   pTracker->pLoop->post(COLnewClosure0(DBDpingCallback, Connected, Url, pTracker, pHasAllAccess));
}

static bool DBDtunnelOpen(TUNstateClient* pClient, DBDpingTracker* pTracker) {
   COL_FUNCTION(DBDtunnelOpen);
   COL_VAR(pClient->m_ConnectionId);
   if(pClient->m_ConnectionId > 0) { return true; }
   pTracker->Error.append(DBDtunnelError());
   return false;
}

COLstring DBDaccessFile() { return FILpathAppend(FILworkingDir(), "access"); }

void DBDcheckAccess(bool* pHasAllAccess, TUNstateClient* pClient) {
   COL_FUNCTION(DBDcheckAccess);
   COLthreadPool Pool;
   Pool.start();
   SCKloop Loop(&Pool);
   auto pTracker = new DBDpingTracker();
   pTracker->pLoop = &Loop;
   DBDcreateAccessList(&pTracker->AccessMap);
   pTracker->Servers = pTracker->AccessMap.size();
   *pHasAllAccess = DBDtunnelOpen(pClient, pTracker);
   for(const auto& it : pTracker->AccessMap) {
      Loop.threadPool()->scheduleTask(COLnewClosure0(&DBDpingServer, it->first, pTracker, pHasAllAccess));
   }
   Loop.start();
}
