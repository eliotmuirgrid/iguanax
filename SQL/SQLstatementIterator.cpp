//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SQLstatementIterator
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Thursday, May 11th, 2006 @ 11:37:00 AM
//
//---------------------------------------------------------------------------
#include "SQLprecomp.h"
#pragma hdrstop

#include <SQL/SQLstatementIterator.h>
#include <SQL/SQLerror.h>
#include <SQL/SQLsqlite.h>
#include <COL/COLutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <SQLTE/sqlite3.h>

#include "SQLbusyHandler.h"

void SQLstatementIterator::reset(){
   if (pStatement->handle()){
      ::sqlite3_reset(reinterpret_cast<sqlite3_stmt*>(pStatement->handle()));
   }
}

COLerror SQLstatementIterator::getError(const COLstring& Context){
   try{
      if (pStatement->sqlite()){
         SQLthrowSqliteError(pStatement->sqlite()->sqlite(), pStatement->sqlite()->filePath(), Context, pStatement->statement() );
      } else {
         SQLthrowSqliteError(0, "", Context, pStatement->statement() );
      }
   }
   catch(const COLerror& Exception){
      return Exception;
   }
   return COLerror("",0);
}

bool SQLstatementIterator::stepNext(){
   switch(stepNextInterruptible()){
   case HaveRows:
      return true;
   case NoRows:
      return false;
   case Interrupted:{
         COLerror Exception( getError("fetching row") );
         reset();
         throw Exception;
      }
      break;
   default:
      break;
   }
   return false;
}

COLuint32 SQLstatementIterator::countOfColumn(){
   COLPRECONDITION(pStatement);
   return ::sqlite3_data_count( reinterpret_cast<sqlite3_stmt*>(pStatement->handle()));
}

SQLstatementIterator::IterateResult SQLstatementIterator::stepNextInterruptible(){
   COL_STATIC_ASSERT(SQLITE_OK == 0);
   sqlite3_stmt* pStatementHandle = reinterpret_cast<sqlite3_stmt*>(pStatement->handle());
   COLuint32 ReturnCode = SQLITE_OK;
   const char* pErrorContext = "fetching row";
   COL_DBG(COL_T("stepNext: ") << sqlite3_sql(pStatementHandle));
   do {
      bool QuitLoop = false;
      ReturnCode = ::sqlite3_step(pStatementHandle);
      switch(ReturnCode){
      case SQLITE_OK:
         break;
      case SQLITE_DONE:
         return NoRows;
         break;
      case SQLITE_ROW:
         return HaveRows;
         break;
      case SQLITE_LOCKED:
      case SQLITE_SCHEMA:
         //Note that this relies on the assumption that
         //we get locked on the very first call to sqlite3_step, so it's ok
         //to call reset (actually it's necessary).
         reset();
         //fall through and sleep
      case SQLITE_BUSY:
         if (ReturnCode == SQLITE_BUSY){
            if (!sqlite3_get_autocommit(pStatement->sqlite()->sqlite())
                && !SQLstatementIsCommit(pStatement->statement()))
            {
               //bail, we have to quit since we're in the 
               //middle of a transaction. This is awkward, but
               //it is as specified in the sqlite docs. We can't retry here
               //else we could just loop forever.
               pErrorContext = "fetching row, and the database is locked within a transaction.\n"
                               "The transaction should be rolled back, and possibly retried at a different time.\n"
                               "Consider BEGIN EXCLUSIVE or BEGIN IMMEDIATE transaction as a possible fix";
               QuitLoop = true;
               break;               
            }
         }
         // To prevent overworking the CPU when the database file is "busy"
         COLsleep(10);
         break;
      case SQLITE_INTERRUPT:
         reset();
         return Interrupted;
         break;
      default:
         break;
      }
      if (QuitLoop) break;
   } while( ReturnCode == SQLITE_BUSY || ReturnCode == SQLITE_LOCKED || ReturnCode == SQLITE_SCHEMA);
   
   if (ReturnCode){
      COLerror Exception( getError(pErrorContext) );
      reset();
      throw Exception;
   }
   return NoRows;
}

SQLstatementIterator::IterateResult SQLstatementIterator::stepNextBusy(SQLbusyHandler& Handler){
   if (!pStatement->handle()){
      SQLthrowSqliteError(0,"","fetching row, the statement has not been prepared.","");
   }

   sqlite3_stmt* pStatementHandle = reinterpret_cast<sqlite3_stmt*>(pStatement->handle());
   COLuint32 ReturnCode = SQLITE_OK;

   COL_DBG(COL_T("stepNextBusy: ") << sqlite3_sql(pStatementHandle));
   COLuint32 CurrentStep = 1;
   do{
      ReturnCode = ::sqlite3_step( pStatementHandle );
      switch(ReturnCode)
      {
      case SQLITE_DONE:
         return NoRows;
         break;
      case SQLITE_ROW:
         return HaveRows;
         break;
      case SQLITE_LOCKED:
         reset();
         //fallthrough
      case SQLITE_BUSY:
         {
            COLsleep(Handler.sqliteBusyInterval());
            //throwing here is ok.
            if (Handler.onSqliteBusy(CurrentStep, sqlite3_get_autocommit(pStatement->sqlite()->sqlite()) != 0)){
               CurrentStep++;
            } else{
               return Interrupted;
            }
         }
         break;
      case SQLITE_OK:
         return NoRows;
         break;
      default:
         break;
      }
   }
   while (ReturnCode == SQLITE_BUSY || ReturnCode == SQLITE_LOCKED);

   //else must have hit an error
   COLerror Exception( getError("fetching row") );
   reset();
   throw Exception;
   return NoRows;
}

COLuint32 SQLstatementIterator::blobValue( COLuint32 ColumnIndex, const void** ppBlobValue ){
   *ppBlobValue = ::sqlite3_column_blob( reinterpret_cast<sqlite3_stmt*>(pStatement->handle()), ColumnIndex );
   return ::sqlite3_column_bytes( reinterpret_cast<sqlite3_stmt*>(pStatement->handle()), ColumnIndex );
}

COLfloat64 SQLstatementIterator::doubleValue(COLuint32 ColumnIndex){
   return ::sqlite3_column_double( reinterpret_cast<sqlite3_stmt*>(pStatement->handle()), ColumnIndex );
}

COLint32 SQLstatementIterator::integerValue( COLuint32 ColumnIndex ){
   return ::sqlite3_column_int( reinterpret_cast<sqlite3_stmt*>(pStatement->handle()), ColumnIndex );
}

COLint64 SQLstatementIterator::integer64Value( COLuint32 ColumnIndex ){
   return ::sqlite3_column_int64( reinterpret_cast<sqlite3_stmt*>(pStatement->handle()), ColumnIndex );
}

int SQLstatementIterator::textValue( COLuint32 ColumnIndex, const char** ppTextValue ){
   // Text is bound as "const char*", should be ok casting from "const unsigned char*"
   *ppTextValue = (const char*) ::sqlite3_column_text( reinterpret_cast<sqlite3_stmt*>(pStatement->handle()), ColumnIndex );
   return ::sqlite3_column_bytes( reinterpret_cast<sqlite3_stmt*>(pStatement->handle()), ColumnIndex );
}

COLstring SQLstatementIterator::textValueStr(COLuint32 ColumnIndex){
   const char* pTmp = 0;
   int len = textValue(ColumnIndex,&pTmp);
   return COLstring(pTmp, len);
}

bool SQLstatementIterator::isNullValue(COLuint32 ColumnIndex){
   return columnType(ColumnIndex) == SQLITE_NULL;
}

int SQLstatementIterator::columnType(COLuint32 ColumnIndex) const{
   return ::sqlite3_column_type(reinterpret_cast<sqlite3_stmt*>(pStatement->handle()), ColumnIndex);
}

