//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SQLerror
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Thursday, May 11th, 2006 @ 11:23:30 AM
//
//---------------------------------------------------------------------------
#include "SQLprecomp.h"
#pragma hdrstop

#include "SQLerror.h"
#include "SQLsqlite.h"
#include <SQLTE/sqlite3.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#define SQL_EXT_CODE_CASE_BLOCK( __EXTCODE )\
   case __EXTCODE:\
      return #__EXTCODE;\
      break

COLstring SQLerrorCodeKey(){return "sqlite3_errcode";}
COLstring SQLschemaChangedErrorCode(){ return COLintToString(SQLITE_SCHEMA); }
COLstring SQLioErrorCode(){return COLintToString(SQLITE_IOERR);}
COLstring SQLdiskFullErrorCode(){return COLintToString(SQLITE_FULL);}

static const char* SQLextCodeToStr(int ExtCode){
   switch(ExtCode){
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_READ);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_SHORT_READ);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_WRITE);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_FSYNC);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_DIR_FSYNC);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_TRUNCATE);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_FSTAT);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_UNLOCK);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_RDLOCK);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_DELETE);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_BLOCKED);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_NOMEM);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_ACCESS);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_CHECKRESERVEDLOCK);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_LOCK);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_CLOSE);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_IOERR_DIR_CLOSE);
   SQL_EXT_CODE_CASE_BLOCK(SQLITE_LOCKED_SHAREDCACHE);
   default:
      break;
   }
   return 0;
}

void SQLthrowSqliteErrorImpl( const COLstring& FilePath, const COLstring& ContextString, const COLstring& Statement, const COLstring& Error, const COLstring& ErrorExtended, COLint32 SqliteReturnCode){
   COL_FUNCTION(SQLthrowSqliteErrorImpl);
   COL_VAR4(FilePath, ContextString, Statement, Error);
   COL_VAR2(ErrorExtended, SqliteReturnCode);
   COLstring ErrorMessage;
   COLostream ErrorStream( ErrorMessage );
#ifdef _WIN32
   ErrorStream.setNewLine("\r\n");
#endif
   ErrorStream << COL_T("SQLite error occurred while ") << ContextString << '.' << newline;
   if (Error.size()){
      ErrorStream << COL_T("SQL error: ") << Error << '.' << newline;
   }
   if (FilePath.size()){
      ErrorStream << COL_T("SQLite: ") << FilePath << newline;
   }
   if (Statement.size()){
      ErrorStream << COL_T("Prepared SQL statement: ") << Statement << newline;
   }
   if (ErrorExtended.size()){
      ErrorStream << newline << COL_T("Extended err code: ") << ErrorExtended;
   }
   if (SqliteReturnCode){
      ErrorStream << newline << COL_T("SQLite return code: ") << SqliteReturnCode;
   }
   COLerror ErrorObj(ErrorMessage, COLerror::SystemError);
   if (SqliteReturnCode){
      ErrorObj.setCode(SqliteReturnCode);
   }
   throw ErrorObj;
}

void SQLthrowSqliteError( void* ipSqliteHandle, const COLstring& FilePath, const COLstring& ContextString, const COLstring& Statement, char* pExecSqliteError){   
   // Notice the error message is stored in the actual SQLite database file.  It's global
   // to that file with all those implications.
   sqlite3* pSqliteHandle = (sqlite3*)ipSqliteHandle;
   COLint32 SqliteReturnCode = 0;
   COLstring SqliteError,SqliteExtendedError;
   if (pSqliteHandle){
      SqliteReturnCode = ::sqlite3_errcode(pSqliteHandle);
      if (pExecSqliteError){
         SqliteError = pExecSqliteError;
      }
      else{
         SqliteError = ::sqlite3_errmsg( pSqliteHandle );
      }
      const char* pExtCodeStr = SQLextCodeToStr(::sqlite3_extended_errcode(pSqliteHandle));
      if (pExtCodeStr){
         SqliteExtendedError = pExtCodeStr;
      }
   }
   
   if (pExecSqliteError){
      ::sqlite3_free( pExecSqliteError );
   }

   SQLthrowSqliteErrorImpl(FilePath, ContextString, Statement, SqliteError, SqliteExtendedError, SqliteReturnCode);
}

bool SQLstatementIsCommit(const COLstring& In)
{
   COLstring ThisStatement = COLstripWhiteSpace(In,true,true);
   ThisStatement.toLowerCase();
   return ThisStatement.find("commit") == 0;
}
