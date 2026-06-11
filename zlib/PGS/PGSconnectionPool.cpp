//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: PGSconnectionPool
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include "PGSconnectionPool.h"
#include "PGSclient.h"

#include <COL/COLlocker.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Uncomment to disable DB connection pooling
//#define PGS_NO_POOL

static void Disconnect(PQconn* pConnection) {
   COL_FUNCTION(Disconnect);
   COL_VAR(pConnection);
   if (!pConnection) { return; }
   (*pPQFinish)(pConnection);
   pConnection = NULL;
}

PGSconnectionPool::PGSconnectionPool() : m_ConnectionCount(0), m_MaxCount(0), m_ConnectionsMade(0){
   COL_METHOD(PGSconnectionPool::PGSconnectionPool);
}

PGSconnectionPool::~PGSconnectionPool(){
   COL_METHOD(PGSconnectionPool::~PGSconnectionPool);
   COL_VAR2(m_ConnectionCount, m_MaxCount);
   shutdown();
}

// TODO - we could transfer this out of this file to seperate this concern out - tracing would be easier.
static PQconn* Connect(const COLvar& ConnectInfo){
   COL_FUNCTION(Connect);
   PQconn* pConnection = NULL;
   // IMPORTANT: the two const char* arrays here must be initialized to length N + 1, where N is the maximum 
   // number of arguments that can be passed in the client connection object. At the moment we never have more 
   // than 5 parameters: dbname, user, password, host, and options. So, if you are changing the options that
   // we want to enable, and pass into the pPQConnect() call below, then you also need to increase the size
   // of the Keys[] and Vals[] initializations accordingly.
   // TODO - if wanted to improve the code quality using a more flexible data-structure would be good.
   const char* Keys[6]; // At the moment we never have more than 5 parameters
   const char* Vals[6];
   COLstring Database, User, Password, Host, Options;
   int i=0;
   if (ConnectInfo.exists("database")){ Keys[i]= "dbname";   Database = ConnectInfo["database"]; Vals[i] = Database.c_str();  i++;}
   if (ConnectInfo.exists("user"))    { Keys[i]= "user";     User     = ConnectInfo["user"];     Vals[i] = User.c_str();      i++;}
   if (ConnectInfo.exists("password")){ Keys[i]= "password"; Password = ConnectInfo["password"]; Vals[i] = Password.c_str();  i++;}
   if (ConnectInfo.exists("host"))    { Keys[i]= "host";     Host     = ConnectInfo["host"];     Vals[i] = Host.c_str();      i++;}
   if (ConnectInfo.exists("options")) { Keys[i]= "options";  Options  = ConnectInfo["options"];  Vals[i] = Options.c_str();   i++;}
   Keys[i] = NULL; Vals[i] = NULL;  // Null terminate our arrays.
   for (int i=0; Keys[i] != NULL; i++){ COL_VAR3(i, Keys[i], Vals[i]); }  // Debug code.
   pConnection = (*pPQConnect)(Keys, Vals, 0);
   COL_VAR(pConnection);
   if (!pConnection) {
      COL_TRC("No connection returned.");
      return NULL;
   }
   if ((*pPQStatus)(pConnection) != 0){
      COL_TRC("Bad connection.");
      const char* pErrorMessage = (*pPQErrorMessage)(pConnection);
      COL_ERR(pErrorMessage);
      (*pPQFinish)(pConnection);  // Cannot reuse connection
      return NULL;
   }
   COL_TRC("Connected to postgres database: " << pConnection);
   return pConnection;
}

PQconn* PGSconnectionPool::reuseConnection(){
   COL_METHOD(PGSconnection::reuseConnection);
   COLlocker Lock(m_PoolLock);
   COL_VAR(m_Connections.size());
   if (m_Connections.size() == 0){
      return NULL;
   }
   PQconn* pConnection =  m_Connections.back();
   m_Connections.pop_back();
   COL_VAR(pConnection);
   return pConnection;
}

PQconn* PGSconnectionPool::connect(const COLvar& ConnectInfo){
   COL_METHOD(PGSconnectionPool::connect);
   PQconn* pConnection = reuseConnection();
   bool Reused = pConnection != NULL;
   if (!pConnection){
      COL_TRC("Need to create a new connection.");
      pConnection = Connect(ConnectInfo);
      COL_VAR(pConnection);
   }
   if (!pConnection){
      COL_TRC("No connection so returning.");
      return NULL;
   }
   COLlocker Lock(m_PoolLock);
   if (!Reused){
      m_ConnectionsMade++;
   }
   m_ConnectionCount++;
   m_MaxCount = COL_MAX(m_MaxCount, m_ConnectionCount);
   COL_VAR3(m_ConnectionCount, m_MaxCount, pConnection);
   return pConnection;
}

void PGSconnectionPool::disconnect(PQconn* pConnection, bool Force){
   COL_METHOD(PGSconnectionPool::disconnect);
   if (pConnection && ((pPQStatus(pConnection) != 0) || strcmp(pPQErrorMessage(pConnection), "") != 0)) {
      COL_TRC("Bad connection.");
      (*pPQFinish)(pConnection);
      pConnection = NULL;
   } 
#ifdef PGS_NO_POOL
   else {
      COL_TRC("Close connection always.");
      Disconnect(pConnection);
   }
#else // ! PGS_NO_POOL
   if (Force) {
      COL_TRC("Force close connection without saving");
      Disconnect(pConnection);
      m_ConnectionCount--;
      return;
   }
#endif // PGS_NO_POOL
   
   COLlocker Lock(m_PoolLock);
   if (pConnection){
      COL_TRC("Saving connection.");
      COL_VAR(pConnection);
      m_Connections.push_front(pConnection);
   }
   m_ConnectionCount--;
   COL_VAR(m_ConnectionCount);
} 

COLstring PGSconnectionPool::report(){
   COL_METHOD(PGSconnectionPool::report);
   COLsinkString Sink;
   COLostream Stream(Sink);
   COLlocker Locker(m_PoolLock);
   Stream << "Max Connections = " << m_MaxCount << newline;
   Stream << "Connections Made = " << m_ConnectionsMade << newline;

   return Sink.string();
}
void PGSconnectionPool::shutdown(){
   COL_METHOD(PGSconnectionPool::shutdown);
   COLlist<PQconn*>::iterator i = m_Connections.begin(), E= m_Connections.end();
   for (; i != E; i++){
      Disconnect(*i);      
   }
   COL_TRC("Clear connections.");
   m_Connections.clear();
   m_ConnectionCount = 0;
   m_ConnectionsMade = 0;
   m_MaxCount = 0;
}
