//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: PGSasyncClient
//
// Author: Eliot Muir
//
// Description:
//
// This is a asynchronous client designed to work with SCKloop.  
//---------------------------------------------------------------------------

#include "PGSasyncClient.h"
#include "PGSclient.h"

#include <SCK/SCKloop.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

PGSasyncClient::PGSasyncClient(SCKloop* pLoop, PGSconnectionPool* pPool) : m_pLoop(pLoop), m_pConnectionPool(pPool) {
   COL_METHOD(PGSasyncClient::PGSasyncClient);
}

PGSasyncClient::~PGSasyncClient(){
   COL_METHOD(PGSasyncClient::~PGSasyncClient);
}
   
void PGSasyncClient::connect(const COLvar& ConnectInfo){
   COL_METHOD(PGSasyncClient::connect);
   m_ConnectInfo = ConnectInfo;
   COL_VAR(m_ConnectInfo);
}

static void DoneOperation(COLauto<COLclosure1< COLauto<COLvar> > > pCallback, COLauto<COLvar> pResult){
   COL_FUNCTION(DoneOperation);
   COL_VAR(*pResult);
   pCallback->run(pResult);
}
   
static void DoExecute(COLauto<COLvar> pCall,  COLauto<COLclosure1< COLauto<COLvar> > > pCallback, SCKloop* pLoop, PGSconnectionPool* pPool){
   COL_FUNCTION(DoExecute);
   COL_VAR(*pCall);
   COLauto<COLvar> pResult = new COLvar();
   try{
      PGSclient Connection(pPool);
      Connection.connect((*pCall)["connect"]);
      Connection.execute((*pCall)["sql"]);
      (*pResult)["success"] = true;
   } catch (const COLerror& Error){
     (*pResult)["success"] = false;
     (*pResult)["error"]   = Error.description();
   }
   COL_VAR(*pResult);  
   pLoop->post(COLnewClosure0(&DoneOperation, pCallback, pResult));
}

// Executes a SQL statement - could be a INSERT, CREATE USER etc.
// The callback returns a COLvar with the expected fields:
//     success - boolean - true or false
//     error   - The text of an error if and only if success == false
void PGSasyncClient::execute(const COLstring& Sql, COLauto<COLclosure1< COLauto<COLvar> > > pCallback){
   COL_METHOD(PGSasyncClient::execute);
   COLauto<COLvar> pCall = new COLvar;
   (*pCall)["sql"] = Sql;
   (*pCall)["connect"] = m_ConnectInfo;
   m_pLoop->threadPool()->scheduleTask(COLnewClosure0(DoExecute, pCall, pCallback, m_pLoop, m_pConnectionPool));
}

// Action happens on a child thread.
static void DoQuery(COLauto<COLvar> pCall,  COLauto<COLclosure1< COLauto<COLvar> > > pCallback, SCKloop* pLoop, PGSconnectionPool* pPool){
   COL_FUNCTION(DoQuery);
   COL_VAR(*pCall);
   COLauto<COLvar> pResult = new COLvar();
   try{
      PGSclient Connection(pPool);
      Connection.connect((*pCall)["connect"]);
      Connection.query((*pCall)["sql"], pResult);
      (*pResult)["success"] = true;
   } catch (const COLerror& Error){
     (*pResult)["success"] = false;
     (*pResult)["error"]   = Error.description();
   }
   COL_VAR(*pResult);  
   pLoop->post(COLnewClosure0(&DoneOperation, pCallback, pResult));
}

// Perform a SQL query - e.g. SELECT
// The callback returns a COLvar with the expected fields
//     success - boolean - true or false
//     error   - The text of an error if and only if success == false
//     data    - The data as returned by PSGclient::query
void PGSasyncClient::query(const COLstring& Sql , COLauto<COLclosure1< COLauto<COLvar> > >  pCallback){
   COL_METHOD(PGSasyncClient::query);
   COLauto<COLvar> pCall = new COLvar;
   (*pCall)["sql"] = Sql;
   (*pCall)["connect"] = m_ConnectInfo;
   m_pLoop->threadPool()->scheduleTask(COLnewClosure0(DoQuery, pCall, pCallback, m_pLoop, m_pConnectionPool));
}

static void DoInsert(COLauto<COLvar> pCall,  COLauto<COLclosure1< COLauto<COLvar> > > pCallback, SCKloop* pLoop, PGSconnectionPool* pPool){
   COL_FUNCTION(DoInsert);
   COL_VAR(*pCall);
   COLauto<COLvar> pResult = new COLvar();
   try{
      PGSclient Connection(pPool);
      Connection.connect((*pCall)["connect"]);
      Connection.insert((*pCall));
      (*pResult)["success"] = true;
   } catch (const COLerror& Error){
     (*pResult)["success"] = false;
     (*pResult)["error"]   = Error.description();
   }
   COL_VAR(*pResult);  
   pLoop->post(COLnewClosure0(&DoneOperation, pCallback, pResult));
}

void PGSasyncClient::insert(COLauto<COLvar> pData, COLauto<COLclosure1< COLauto<COLvar> > > pCallback){
   COL_METHOD(PGSasyncClient::insert);
    (*pData)["connect"] = m_ConnectInfo;
    m_pLoop->threadPool()->scheduleTask(COLnewClosure0(DoInsert, pData, pCallback, m_pLoop, m_pConnectionPool));
}

static void DoCopy(COLauto<COLvar> pCall,  COLauto<COLclosure1< COLauto<COLvar> > > pCallback, SCKloop* pLoop, PGSconnectionPool* pPool){
   COL_FUNCTION(DoCopy);
   COL_VAR(*pCall);
   COLauto<COLvar> pResult = new COLvar();
   try{
      PGSclient Connection(pPool);
      Connection.connect((*pCall)["connect"]);
      Connection.copy((*pCall)["sql"], pResult);
      (*pResult)["success"] = true;
   } catch (const COLerror& Error){
     (*pResult)["success"] = false;
     (*pResult)["error"]   = Error.description();
   }
   COL_VAR(*pResult);  
   pLoop->post(COLnewClosure0(&DoneOperation, pCallback, pResult));
}

void PGSasyncClient::copy(const COLstring& Sql, COLauto<COLclosure1< COLauto<COLvar> > > pCallback){
   COL_METHOD(PGSasyncClient::copy);
   COLauto<COLvar> pCall = new COLvar;
   (*pCall)["sql"] = Sql;
   (*pCall)["connect"] = m_ConnectInfo;
   m_pLoop->threadPool()->scheduleTask(COLnewClosure0(DoCopy, pCall, pCallback, m_pLoop, m_pConnectionPool));
}