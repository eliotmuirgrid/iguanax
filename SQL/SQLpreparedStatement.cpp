//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SQLpreparedStatement
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

#include "SQLpreparedStatement.h"
#include "SQLsqlite.h"
#include "SQLerror.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <COL/COLutils.h>

#include <SQLTE/sqlite3.h>

static const COLuint32 BUSY_WAIT_EVENT_TIMEOUT = 10;

SQLpreparedStatement::SQLpreparedStatement() : pSqliteStatement(0), pSqlite(0), pIterator(0){}

SQLpreparedStatement::~SQLpreparedStatement(){
   finalize();
}

const COLstring& SQLpreparedStatement::statement() const{
   return Statement;
}

void* SQLpreparedStatement::handle(){
   return pSqliteStatement;
}

SQLsqlite* SQLpreparedStatement::sqlite(){
   return pSqlite;
}

void SQLpreparedStatement::prepare( SQLsqlite& Sqlite, const char* iStatement ){
   if (!prepareInterruptible(Sqlite,iStatement)){
      SQLthrowSqliteError( Sqlite.sqlite(), Sqlite.filePath(),"preparing SQL statement",iStatement );    
   }
}

bool SQLpreparedStatement::prepareInterruptible(SQLsqlite& Sqlite, const char* iStatement){
   COLPRECONDITION(pSqlite == 0)
   COLPRECONDITION(Statement.is_null())
   COLPRECONDITION(pSqliteStatement == 0)

   COLuint32 ReturnCode = SQLITE_OK;   
   sqlite3_stmt* pTempSqliteStatement = 0;
   do{
      pTempSqliteStatement = 0;
      ReturnCode = ::sqlite3_prepare_v2( Sqlite.sqlite(), 
                           iStatement, 
                           -1, // No length specified, statement ends at null terminator
                           &pTempSqliteStatement, 
                           NULL ); // Points to rest of string if statement length was specified
      if ( ReturnCode == SQLITE_BUSY || ReturnCode == SQLITE_LOCKED){
         // To prevent overworking the CPU when the database file is "busy"
         COLsleep(BUSY_WAIT_EVENT_TIMEOUT);
      }
   } while(ReturnCode == SQLITE_BUSY || ReturnCode == SQLITE_LOCKED);
   
   if (ReturnCode != SQLITE_OK && ReturnCode != SQLITE_INTERRUPT){
      SQLthrowSqliteError( Sqlite.sqlite(), Sqlite.filePath(), "preparing SQL statement",iStatement );
   }

   if (ReturnCode == SQLITE_INTERRUPT){
      COL_TRC(COL_T("Prepared statement interrupted : ") << pSqliteStatement << COL_T(" this: ") << this);
      return false;
   }

   if (!pTempSqliteStatement || strlen(iStatement) == 0)
   {
      SQLthrowSqliteErrorImpl(Sqlite.filePath(), "preparing SQL statement",iStatement,"Statement cannot be empty","",0);
   }
   COLPOSTCONDITION(pTempSqliteStatement != 0)
   //ok, state is safe, we save the statement and the sqlite handle
   pSqlite = &Sqlite;
   pSqlite->registerPreparedStatement(this);
   Statement = iStatement;
   pSqliteStatement = pTempSqliteStatement;   
   COL_TRC(COL_T("Prepared statement with handle: ") << pSqliteStatement << COL_T(" this: ") << this);

   return true;
}

void SQLpreparedStatement::finalize(){  
   if ( pSqliteStatement != 0 ){
      ::sqlite3_reset( reinterpret_cast<sqlite3_stmt*>(pSqliteStatement));
      ::sqlite3_finalize(reinterpret_cast<sqlite3_stmt*>(pSqliteStatement));
      COL_TRC(COL_T("Finalized statement with handle: ") << pSqliteStatement << COL_T(" this: ") << this);
      pSqliteStatement = 0;
      Statement.clear();
      pSqlite->unregisterPreparedStatement(this);
      pSqlite = 0;
   }
}

void SQLpreparedStatement::handleError(const COLstring& Context){
   if (pSqlite){
      SQLthrowSqliteError(pSqlite->sqlite(),pSqlite->filePath(),Context,Statement,0);
   } else {
      SQLthrowSqliteError(0,"",Context,Statement,0);
   }
}

void SQLpreparedStatement::bindBlobValue( COLuint32 ColumnIndex, const void* pBlobValue, COLuint32 SizeOfBlob ){
   if (::sqlite3_bind_blob(reinterpret_cast<sqlite3_stmt*>(pSqliteStatement), ColumnIndex, pBlobValue, SizeOfBlob, SQLITE_STATIC) != SQLITE_OK ){
      COLstring ContextString;
      COLostream ContextStream( ContextString );

      COLstring BlobString((const char*)pBlobValue, SizeOfBlob);

      ContextStream << COL_T("binding blob value '") << BlobString << COL_T("' (") << SizeOfBlob << COL_T(" bytes) to column ") << ColumnIndex; 
      handleError( ContextString );
   }
}

void SQLpreparedStatement::bindNullValue( COLuint32 ColumnIndex){
   if ( ::sqlite3_bind_null(reinterpret_cast<sqlite3_stmt*>(pSqliteStatement), ColumnIndex) != SQLITE_OK ){
      COLstring ContextString;
      COLostream ContextStream( ContextString );

      ContextStream << COL_T("binding null value to column ") << ColumnIndex; 
      handleError( ContextString );
   }
}

void SQLpreparedStatement::bindDoubleValue( COLuint32 ColumnIndex, COLfloat64 DoubleValue ){
   if ( ::sqlite3_bind_double(reinterpret_cast<sqlite3_stmt*>(pSqliteStatement), ColumnIndex, DoubleValue) != SQLITE_OK ){
      COLstring ContextString;
      COLostream ContextStream( ContextString );
      ContextStream << COL_T("binding double value ") << DoubleValue << COL_T(" to column ") << ColumnIndex; 
      handleError( ContextString );
   }
}

void SQLpreparedStatement::bindIntegerValue(COLuint32 ColumnIndex, COLint32 IntegerValue){
   if ( ::sqlite3_bind_int(reinterpret_cast<sqlite3_stmt*>(pSqliteStatement), ColumnIndex, IntegerValue) != SQLITE_OK){
      COLstring ContextString;
      COLostream ContextStream( ContextString );
      ContextStream << COL_T("binding integer value ") << IntegerValue << COL_T(" to column ") << ColumnIndex; 
      handleError( ContextString );
   }
}

void SQLpreparedStatement::bindInteger64Value( COLuint32 ColumnIndex, COLint64 IntegerValue ){
   if ( ::sqlite3_bind_int64(reinterpret_cast<sqlite3_stmt*>(pSqliteStatement), ColumnIndex, IntegerValue) != SQLITE_OK ){
      COLstring ContextString;
      COLostream ContextStream( ContextString );
      ContextStream << COL_T("binding 64-bit integer value ") << IntegerValue << COL_T(" to column ") << ColumnIndex; 
      handleError( ContextString );
   }
}

void SQLpreparedStatement::bindTextValue(COLuint32 ColumnIndex, const char* pTextValue, COLuint32 SizeOfText){
   if ( ::sqlite3_bind_text(reinterpret_cast<sqlite3_stmt*>(pSqliteStatement), ColumnIndex, pTextValue, SizeOfText, SQLITE_STATIC) != SQLITE_OK ){
      COLstring ContextString;
      COLostream ContextStream( ContextString );

      COLstring TextString( pTextValue, SizeOfText );

      ContextStream << COL_T("binding text value '") << TextString << COL_T("' to column ") << ColumnIndex; 
      handleError( ContextString );
   }
}

COLuint32  SQLpreparedStatement::countOfColumn() const{
   return ::sqlite3_column_count( reinterpret_cast<sqlite3_stmt*>(pSqliteStatement) );
}

const char* SQLpreparedStatement::columnName(COLuint32 ColumnIndex) const{
   return ::sqlite3_column_name( reinterpret_cast<sqlite3_stmt*>(pSqliteStatement), ColumnIndex);
}

void SQLpreparedStatement::setIterator(const SQLstatementIterator* pIterator){
   pIterator = pIterator;
}

const SQLstatementIterator* SQLpreparedStatement::iterator() const{
   return pIterator;
}
