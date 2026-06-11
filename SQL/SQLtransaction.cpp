//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SQLtransaction
//
// Description:
//
// Implementation.
//
// Author:   Henry Tran, Yuriy Kachanov
// Date:     September 2005
//
//---------------------------------------------------------------------------
#include "SQLprecomp.h"
#pragma hdrstop

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "SQLtransaction.h"
#include "SQLsqlite.h"

SQLtransaction::SQLtransaction(SQLsqlite& Sqlite, const char* apType ) : m_Sqlite(Sqlite), m_Committed(false){
   Sqlite.executeSqlString( apType ); // ok to throw since member destructors don't need to be called.
}

SQLtransaction::~SQLtransaction(){
   try{
      if (!m_Committed) { m_Sqlite.executeSqlString("ROLLBACK"); }
   } catch(const COLerror& Error){     //swallow exceptions, we're in a destructor
      COL_WRN(COL_T("Exception thrown from destructor in SQLtransaction: ") << Error);
   }
}

void SQLtransaction::commit(){
   m_Sqlite.executeSqlString("COMMIT");
   m_Committed = true;
}

SQLimmediateTransaction::SQLimmediateTransaction( SQLsqlite& iSqlite ) : SQLtransaction(iSqlite, "BEGIN IMMEDIATE"){}
SQLexclusiveTransaction::SQLexclusiveTransaction( SQLsqlite& iSqlite ) : SQLtransaction(iSqlite, "BEGIN EXCLUSIVE"){}



