//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBtransaction
//
// Description:
//
// Implementation.
//
// Author: Henry Tran
// Date:   April 13, 2004 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBtransaction.h"
#include "DBdatabase.h"
#include "DBdatabasePtr.h"
#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBbeginTransaction(DBdatabase* pDatabase){
   COL_FUNCTION(DBbeginTransaction);
   COLASSERT(pDatabase != NULL);
   // TODO Eliot - it seems questionable to me that this logic exists here.  For ODBC the APIs
   // would throw by themselves if we were not connected.  Seems better to let each DB implementation
   // directly implement beginTransaction. 
   if (!pDatabase->isConnected()){
      COL_ERR("Not connected so cannot begin transaction.");
      COL_ERROR_STREAM_PLAIN("Cannot make database transaction because connection is not ready.",COLerror::PreCondition)
   } else if (pDatabase->transactionActive()){
      COL_ERR("Cannot make database transaction because an active transaction already exists.");
      COL_ERROR_STREAM("Cannot make database transaction because an active transaction already exists.", COLerror::PreCondition)
   }
   pDatabase->beginTransaction();
}

DBtransaction::DBtransaction(DBdatabase* ipDatabase) : CommitFlag(false), pDatabase(ipDatabase){
   COL_METHOD(DBtransaction::DBtransaction);
   beginTransaction();
}

DBtransaction::DBtransaction(DBdatabase& Database) : CommitFlag(false), pDatabase(&Database){
   COL_METHOD(DBtransaction::DBtransaction);
   beginTransaction();
}

void DBtransaction::beginTransaction(){
   COL_METHOD(beginTransaction);
   DBbeginTransaction(pDatabase);
}

DBtransaction::~DBtransaction(){
   COL_METHOD(DBtransaction::~DBtransaction);
   try {
      //this ensures that we do not rollback if the database is in a disconnected state.
      if (CommitFlag == false && pDatabase->isConnected()) {
         rollback();
      }
   }
   catch(const COLerror& Error) {
      COL_ERR(Error);
   }
}

void DBtransaction::commit(){
   COL_METHOD(commit);
   pDatabase->commitTransaction();
   CommitFlag = true;
}

void DBtransaction::rollback(){
   COL_METHOD(rollback);
   pDatabase->rollbackTransaction();
}
