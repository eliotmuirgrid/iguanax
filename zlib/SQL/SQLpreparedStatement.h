#ifndef __SQL_PREPARED_STATEMENT_H__
#define __SQL_PREPARED_STATEMENT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SQLpreparedStatement
//
// Description:
//
// Wrapper to simplify the process of preparing (i.e. binding) SQL statements
// for SQLite. 
//
// Author:   Henry Tran, Yuriy Kachanov
// Date:     September 2005
//
//---------------------------------------------------------------------------

class SQLsqlite;
class SQLstatementIterator;

class SQLpreparedStatement{
public:
   SQLpreparedStatement();
   ~SQLpreparedStatement();

   // Prepare the statement in SQLite
   bool prepareInterruptible(SQLsqlite& iSqlite, const char* Statement);
   void prepare( SQLsqlite& iSqlite, const char* Statement );

   // null if the statement has finalized
   const COLstring& statement() const;
   
   // null if the statement has finalized
   SQLsqlite* sqlite();

   //the sqlite3 statement handle, null if not prepared or has been finalized
   void* handle();

   void bindBlobValue( COLuint32 ColumnIndex, const void* pBlobValue, COLuint32 SizeOfBlob );
   void bindDoubleValue( COLuint32 ColumnIndex, COLfloat64 DoubleValue );
   void bindIntegerValue( COLuint32 ColumnIndex, COLint32 IntegerValue );
   void bindInteger64Value( COLuint32 ColumnIndex, COLint64 IntegerValue );
   void bindTextValue( COLuint32 ColumnIndex, const char* pTextValue, COLuint32 SizeOfText );
   void bindNullValue( COLuint32 ColumnIndex);

   // no throw. Resets the prepared statement. All iterators using this statement are invalidated
   void finalize();

   // count of columns in the statement
   COLuint32 countOfColumn() const;

   const char* columnName(COLuint32 ColumnIndex) const;

   friend class SQLstatementIterator;

private:
   void handleError(const COLstring& Context);
   
   //this is used by an iterator to determine if the current
   //statement already has an iterator. This prevents
   //multiple iterators from using the same statement at the same time.

   //no throw
   void setIterator(const SQLstatementIterator* pIterator);
   const SQLstatementIterator* iterator() const;
   
   void* pSqliteStatement;
   SQLsqlite* pSqlite;    // needed for error log only!
   COLstring     Statement;
   const SQLstatementIterator* pIterator;

   SQLpreparedStatement(const SQLpreparedStatement& Orig);  
   SQLpreparedStatement& operator=(const SQLpreparedStatement& Orig);
};

#endif // end of defensive include
