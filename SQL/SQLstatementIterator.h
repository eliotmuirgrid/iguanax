#ifndef __SQL_STATEMENT_ITERATOR_H__
#define __SQL_STATEMENT_ITERATOR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SQLstatementIterator
//
// Description:
//
// This iterator ensures that reset is called when the iterator is destroyed
//
// Author: Nasron Cheong
// Date:   Thursday, May 11th, 2006 @ 11:37:00 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <SQL/SQLpreparedStatement.h>

class SQLpreparedStatement;
class SQLstatementIteratorPrivate;
class SQLbusyHandler;

class SQLstatementIterator{
public:
   SQLstatementIterator(SQLpreparedStatement& Statement) : pStatement(&Statement){
      COLMSGPRECONDITION(!pStatement->iterator(),"An iterator already exists for the prepared statement.");
   }
   
   enum IterateResult{
      HaveRows,
      NoRows,
      Interrupted
   };

   // This is non virtual because we are assuming no classes will inherit from this class
   // The statement is reset once the destructor is called
   ~SQLstatementIterator(){
      reset();
      pStatement->setIterator(0);
   }

   // Returns true if next available result from the current
   // statement has executed, false if no row data is available. If an 
   // error occurs, the iterator is reset.  This never times out
   bool stepNext();

   // Similar to stepNext, but if interrupted is called on the underlying SQLsqlite,
   // or the query progress interrupts, this returns interrupted. 
   IterateResult stepNextInterruptible();

   // If busy, calls the busy handler during the operation, if the handler returns
   // false, Interrupted is returned. If the handler throws it is thrown here.
   IterateResult stepNextBusy(SQLbusyHandler& Handler);

   // resets the iterator to the beginning. No throw.
   void reset();

   //count of columns in a result set
   COLuint32 countOfColumn();

   //returns directly from sqlite3_column_type
   //for current row
   int columnType(COLuint32 ColumnIndex) const;

   // Returns size of blob, SQLite maintains ownership of blob value
   COLuint32 blobValue( COLuint32 ColumnIndex, const void** ppBlobValue );

   // Returns size of text string, SQLite maintains ownership of text value
   int textValue(COLuint32 ColumnIndex, const char** ppTextValue);

   // Same as textValue but returns a copy as a COLstring
   COLstring textValueStr(COLuint32 ColumnIndex);

   COLfloat64 doubleValue( COLuint32 ColumnIndex );
   COLint32 integerValue( COLuint32 ColumnIndex );
   COLint64 integer64Value( COLuint32 ColumnIndex );

   // Note, this must be called before calling any value calls on the column
   // else the result is undefined.
   bool isNullValue(COLuint32 ColumnIndex);
   
private:
   COLerror getError(const COLstring& Context);
   SQLpreparedStatement* pStatement;
   SQLstatementIterator(const SQLstatementIterator& Orig); // not allowed
   SQLstatementIterator& operator=(const SQLstatementIterator& Orig); // not allowed
};

#endif // end of defensive include
