#ifndef __SQL_SQLITE_H__
#define __SQL_SQLITE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SQLsqlite
//
// Description:
//
// Abstract interface for SQLite database files.
//
// Author:   Henry Tran, Yuriy Kachanov
// Date:     September 2005
//
//---------------------------------------------------------------------------

class SQLsqlitePrivate;
struct sqlite3;

class SQLbusyHandler;
class SQLpreparedStatement;

#include <COL/COLmutex.h>

class SQLsqlite
{
public:
   typedef int (*TProgressCallback)(void*);

   SQLsqlite();
   ~SQLsqlite();

   void open(const COLstring& FilePath, const COLstring& EncryptionKey = "");
   bool reKey(const COLstring& OldKey, const COLstring& NewKey);

   //no throw
   void close();

   sqlite3* sqlite() const;

   // Includes directory path
   const COLstring& filePath() const;

   bool isValid();

   void executeSqlString(const char* SqlString);

   //returns false if interrupted
   bool executeSqlStringInterruptible(const char* SqlString);

   //calls the handler onSqlliteBusy function if the operation is busy
   //returns true if the operation succeeded, false if the busy handler interrupted the operation
   //If the handler throws it is thrown here.
   bool executeSqlStringBusy(const char* SqlString, SQLbusyHandler& Handler);

   COLint64 lastInsertRowId();

   friend class SQLpreparedStatement;

   //sends an interrupt to any pending db operation
   //may be called from any other thread.
   void interrupt();

   void setProgressCallback(TProgressCallback Callback, void* pContext);

private:
   COLmutex m_InterruptSection;
   sqlite3*  m_pSqlite;
   COLstring m_FilePath;

   SQLsqlitePrivate* pMember;
   //these are used for debugging
   void  registerPreparedStatement(SQLpreparedStatement* pStatement);
   void  unregisterPreparedStatement(SQLpreparedStatement* pStatement);

   SQLsqlite(const SQLsqlite& Orig);  
   SQLsqlite& operator=(const SQLsqlite& Orig);
};

#endif // end of defensive include
