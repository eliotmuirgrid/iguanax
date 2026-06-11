//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SQLqueue
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

#include <COL/COLhashmap.h>
#include <COL/COLlocker.h>

#include <FIL/FILutils.h>

#include <SQL/SQLsqlite.h>
#include <COL/COLutils.h>
#include <SQL/SQLpreparedStatement.h>

#define SQLITE_WIN32_IOERR_RETRY 44
#define SQLITE_WIN32_IOERR_RETRY_DELAY 30
#define SQLITE_HAS_CODEC 1
#include <SQLTE/sqlite3.h>

#include "SQLbusyHandler.h"
#include "SQLerror.h"


#define SQLITE_ERROR_PREFIX_BUSY_IN_TRANSACTION \
"executing this SQL statement. The database appears locked while this statement is executing within a transaction.\n"\
"The transaction should be rolled back, and possibly retried at a different time.\n"\
"Consider using a BEGIN EXCLUSIVE or BEGIN IMMEDIATE transaction as a possible fix"

static const int WinRetries[] = {SQLITE_WIN32_IOERR_RETRY, SQLITE_WIN32_IOERR_RETRY_DELAY};

class SQLsqlitePrivate{
public:
   SQLsqlitePrivate() {}
   ~SQLsqlitePrivate() {}

   COLhashmap<SQLpreparedStatement*, COLuint32, COLhashPointer<SQLpreparedStatement*> > PreparedStatements;
};

SQLsqlite::SQLsqlite() : m_pSqlite(NULL){
   pMember = new SQLsqlitePrivate();
}

SQLsqlite::~SQLsqlite(){
   COL_METHOD(SQLsqlite::~SQLsqlite);
   close();
   delete pMember;
}

void SQLsqlite::open(const COLstring& FilePath, const COLstring& EncryptionKey) {
   COL_METHOD(SQLsqlite::open);
   COL_VAR(FilePath);
   COL_VAR(EncryptionKey.is_null());
   
   sqlite3* pTempDb = 0;

   COLint32 ReturnCode = SQLITE_OK;
   ReturnCode = ::sqlite3_open(FilePath.c_str(), &pTempDb);
   if (ReturnCode != SQLITE_OK) {
      try {
         SQLthrowSqliteError(pTempDb, FilePath, "opening file", "");
      } 
      catch (...) {
         //no throw
         ::sqlite3_close(pTempDb);
         throw;
      }
   }

   if (!EncryptionKey.is_null()) {
      COL_TRC("Setting encryption key in database...");
      bool SQLsqliteDatabaseEncryptionSupported = false; // Commented out the EncryptionKey logic because it is not used and was causing build errors.
      COLASSERT(SQLsqliteDatabaseEncryptionSupported);
      // int Check = sqlite3_key_v2(pTempDb, NULL, EncryptionKey.c_str(), EncryptionKey.size()); 
      // COL_VAR(Check);
   }
   else {
      COL_TRC("Encryption key not supplied, treating database as unencrypted.");
   }
   
   sqlite3_file_control(pTempDb, NULL, SQLITE_FCNTL_WIN32_AV_RETRY, (void*)&WinRetries);
   COL_TRC("Open was successful.");

   //open ok, we swap into object
   close(); //close currently open handles
   m_FilePath = FilePath; {
      COLlocker Lock(m_InterruptSection);
      m_pSqlite = pTempDb;
   }
   
   COL_TRC("Done!");
}


void SQLsqlite::close() {
   COL_METHOD(SQLsqlite::close);
   bool ValidPointer = m_pSqlite != NULL;
   COL_VAR(ValidPointer);
   
   if (ValidPointer) {
      if (pMember->PreparedStatements.size()){
         COLstring ErrString;
         COLostream ErrStream(ErrString);
         for (COLhashmapPlace Place = pMember->PreparedStatements.first(); Place; Place = pMember->PreparedStatements.next(Place)){
            ErrStream << hex << pMember->PreparedStatements.key(Place) << ',';
         }
         COL_ERR(COL_T("Prepared Statements ") << ErrString << COL_T(" not finalized before closing sqlite db"));
      }
      {
         COLlocker Lock(m_InterruptSection);
         ::sqlite3_close(m_pSqlite);
         m_pSqlite = NULL;
      }
      m_FilePath.clear();
   }
}

sqlite3* SQLsqlite::sqlite() const{
   return m_pSqlite;
}

const COLstring& SQLsqlite::filePath() const{
   return m_FilePath;
}

bool SQLsqlite::isValid() {
   COL_METHOD(SQLsqlite::isValid);
   char* pErrorMsg = NULL;
   
   int RetCode = ::sqlite3_exec(m_pSqlite, "SELECT * from sqlite_master LIMIT 1", NULL, NULL, &pErrorMsg);
   
   COLstring Err(pErrorMsg);
   COL_VAR2(Err, RetCode);

   bool Valid = true;

   if (RetCode == SQLITE_NOTADB) {
      // Either invalid database OR db is encrypted and 
      // was opened without a supplied encryption key.
      Valid = false;
   }
   
   COL_VAR(Valid);
   return Valid;
}

bool SQLsqlite::executeSqlStringInterruptible(const char* SqlString){
   COL_METHOD(SQLsqlite::executeSqlStringInterruptible);
   // NOTE: Queries are not supported in this function
   char* pErrorMessage = NULL;
   COLint32 ReturnCode = SQLITE_OK;
   const char* pErrorPrefix = "executing SQL statement";
   for(;;) {
      COL_DBG(COL_T("executeSqlString: ") << SqlString);
      ReturnCode = ::sqlite3_exec( m_pSqlite,
                                   SqlString, 
                                   NULL, // No callback function to handle queries
                                   NULL, // No arbitrary pointer for (non-existant) callback function
                                   &pErrorMessage ); // Set NULL if OK.
      COL_VAR(ReturnCode);
      COLstring Error(pErrorMessage);
      COL_VAR(Error);
      if ( ReturnCode != SQLITE_BUSY  && 
           ReturnCode != SQLITE_LOCKED &&
           ReturnCode != SQLITE_CANTOPEN)
      {
         break;
      }
      
      if (ReturnCode == SQLITE_BUSY && !SQLstatementIsCommit(SqlString)){
         if (!sqlite3_get_autocommit(m_pSqlite)) {
            pErrorPrefix = SQLITE_ERROR_PREFIX_BUSY_IN_TRANSACTION;
            //bail, we have to quit since we're in the 
            //middle of a transaction. This is awkward, but
            //it is as specified in the sqlite docs. We can't retry here
            //else we could just loop forever.
            break;
         }
      }
      
      sqlite3_free(pErrorMessage);
      pErrorMessage = 0;
      
      // To prevent overworking the CPU when the database file is "busy"
      COLsleep(10);
   };
   
   COL_VAR(pErrorMessage);

   if ( pErrorMessage != NULL  && ReturnCode != SQLITE_INTERRUPT) {
      SQLthrowSqliteError(sqlite(), m_FilePath, pErrorPrefix, SqlString, pErrorMessage);
   }

   sqlite3_free(pErrorMessage);
   return (ReturnCode == SQLITE_OK);
}

void SQLsqlite::executeSqlString(const char* SqlString){
   if (!executeSqlStringInterruptible(SqlString)){
      SQLthrowSqliteError( sqlite(), filePath(), "executing SQL statement", SqlString, 0);
   }
}

bool SQLsqlite::executeSqlStringBusy(const char* SqlString, SQLbusyHandler& Handler){
   // NOTE: Queries are not supported in this function
   char* pErrorMessage = NULL;
   COLint32 ReturnCode = SQLITE_OK;
   COLuint32 BusyStep = 1;
   do{
      COL_DBG(COL_T("executeSqlStringBusy: ") << SqlString);
      ReturnCode = ::sqlite3_exec( m_pSqlite,
                                   SqlString, 
                                   NULL, // No callback function to handle queries
                                   NULL, // No arbitrary pointer for (non-existant) callback function
                                   &pErrorMessage );
      if ( ReturnCode == SQLITE_BUSY  || 
           ReturnCode == SQLITE_LOCKED ||
           ReturnCode == SQLITE_CANTOPEN){
         if ( pErrorMessage != NULL )
         {
            ::sqlite3_free( pErrorMessage );
            pErrorMessage = NULL;
         }
         // Have to handle the busy situation
         COLsleep(Handler.sqliteBusyInterval());
         
         //ok to throw from this callback since error message is freed.
         if (Handler.onSqliteBusy(BusyStep, ::sqlite3_get_autocommit(m_pSqlite) != 0)){
            BusyStep++;
         } else {
            return false;            
         }
      }

   } while ( ReturnCode == SQLITE_BUSY || ReturnCode == SQLITE_CANTOPEN || ReturnCode == SQLITE_LOCKED);

   //not busy
   if ( pErrorMessage != NULL ){
      SQLthrowSqliteError(sqlite(), filePath(), "executing SQL statement", SqlString, pErrorMessage);
   }

   //ok,
   return true;
}

COLint64 SQLsqlite::lastInsertRowId(){
   return ::sqlite3_last_insert_rowid( m_pSqlite );   
}

void SQLsqlite::registerPreparedStatement(SQLpreparedStatement* pStatement){
   pMember->PreparedStatements[pStatement] = 1;
}

void SQLsqlite::unregisterPreparedStatement(SQLpreparedStatement* pStatement){
   pMember->PreparedStatements.removeKey(pStatement);
}

void SQLsqlite::interrupt(){
   COLlocker Lock(m_InterruptSection);
   if (m_pSqlite){
      ::sqlite3_interrupt(m_pSqlite);
   }
}

void SQLsqlite::setProgressCallback(TProgressCallback Callback, void* pContext){
   if (m_pSqlite){
      //TODO - how many ops between callbacks?
      ::sqlite3_progress_handler(m_pSqlite, 10, Callback, pContext);
   }
}

