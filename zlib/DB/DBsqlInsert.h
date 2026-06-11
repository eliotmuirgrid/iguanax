#ifndef __DB_SQL_INSERT_H__
#define __DB_SQL_INSERT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlInsert
//
// Description:
//
// SQL command class for INSERT operations (to insert a row [or more] of 
// data into a database table), which consists of a table name, names of the 
// columns to insert data into, and the values to insert into those columns, 
// all encapsulated in an object.
//
// There are two ways to specify the column values to insert:
// 1) Specify each column name and value manually
// 2) Specify only the column names and utilize the result of a SELECT query 
//    to insert data into the specified columns
//
// Author: Henry Tran
// Date:   November 25, 2003 
//
//---------------------------------------------------------------------------

#include "DBsqlCommand.h"

class DBvariant;
class DBsqlInsertPrivate;
class DBsqlSelect;

class DBsqlInsert :  public DBsqlCommand
{
public:
   DBsqlInsert();   
   DBsqlInsert( const DBsqlInsert& Orig );
   DBsqlInsert& operator=( const DBsqlInsert& Orig ); 
   virtual ~DBsqlInsert();

   virtual DBsqlCommandType commandType() const { return DB_INSERT_CMD; }

   // Set the name of the table where data will be inserted into.
   void setTableName( const COLstring& TableName, bool QuoteName = false );
   
   // Retrieve the name of the table for this INSERT command.
   const COLstring& tableName() const;

   bool quoteTableName() const;

   // Add a column name and value pair to insert into the specified database table.
   // Specify only column names when a SELECT query is utilized for the values to 
   // insert. This function returns the index of the added column.
   const COLuint32 addColumn( const COLstring& ColumnName, 
                              const DBvariant& ColumnValue,
                              const bool& ValueIsColumnFlag = false,
                              bool QuoteColumnName = false,
                              COLindex       NumberOfRows = 1
                            );
   
   // Removes a column by name. Returns if the column was found before removal or not 
   bool removeColumn( const COLstring& ColumnName);

   void resizeColumnVector( COLuint32 Size );
   
   void setColumn( COLuint32 ColumnIndex,
                   const COLstring& ColumnName, 
                   const DBvariant& ColumnValue,
                   const bool& ValueIsColumnFlag = false,
                   bool QuoteColumnName = false );

   // Retrieve the name of a column where data will be inserted into by vector index.
   const COLstring& columnName( COLuint32 ColumnIndex ) const;
   
   // Retrieve the value that will be inserted into the corresponding column
   // by vector index. The type of this DBvariant object may be checked with 
   // the dataType() method. Its value may be converted to the required type 
   // for usage (look at the header file DBvariant.h for more details).
   const DBvariant& columnValue( COLuint32 ColumnIndex, COLuint32 RowIndex = 0 ) const;

   const bool& columnValueIsColumnFlag( COLuint32 ColumnIndex ) const;

   bool quoteColumnName( COLuint32 ColumnIndex ) const;

   // The current number of columns for this table insertion command. Note that
   // both the column name and column value vectors are the same size if column
   // values are explicitly specified (instead of a SELECT query).
   const COLuint32 countOfColumn() const;

   // Optionally add a SELECT query to this INSERT command to insert values from 
   // the query's result set. When using this method of row insertion, only the
   // column names need to be specified in the above column vector. Also note
   // that this method may insert more than one row of data into the table.
   void addSelectQuery();
   
   // Access (and set) the parameters of the SELECT query through the returned
   // reference. Note that the order of the column names in the SELECT query must
   // match the order in the above column vector so that data will be inserted 
   // into the correct columns.
   DBsqlSelect& selectQuery() const;

   // Returns true if a SELECT query has been added to this INSERT command.
   const bool selectQueryExists() const;

   // Support for multi-row inserts
   COLindex countOfRows () const;
   void setColumnValue( COLuint32 Col, COLuint32 Row, const DBvariant& Value);

private:
   DBsqlInsertPrivate* pMember;
};

#endif // end of defensive include
