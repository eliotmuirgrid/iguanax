#ifndef __DB_RESULT_SET_H__
#define __DB_RESULT_SET_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBresultSet
//
// Description:
//
// A class to store a result set from a database query. This is 
// basically a 2-dimensional array: a vector of rows, each of which
// contains a vector of column values.
//
// Additionally, this result set object contains a separate vector for 
// column names.
//
// Author: Henry Tran
// Date:   November 27, 2003 
//
//---------------------------------------------------------------------------

#include <COL/COLrefCounted.h>

#include "DBminimumInclude.h"

class DBresultSetRow;
class DBresultSetPrivate;
class DBvariant;
class DBresultSetPtr;

class DBresultSet : public COLrefCounted {
public:
   DBresultSet(); 
   virtual ~DBresultSet();

   // For performance reasons, you should set the number of columns in the 
   // result set before specifying the details of each column. Also resizes 
   // the column vector of any existing row objects to accommodate the 
   // specified number of columns.
   void resizeColumnVector( COLuint32 CountOfColumn );

   // Set the column name and data type in the specified column index.
   void setColumn( COLuint32 ColumnIndex, const COLstring& NewColumnName, DBdataType NewDataType );

   // Retrieve a result set column name by vector index.
   const COLstring& columnName( COLuint32 ColumnIndex ) const;

   // Retrieve a column data type by vector index.
   DBdataType columnType( COLuint32 ColumnIndex ) const;

   // returns COLuint32(-1) if non existent
   COLuint32 columnIndex(const COLstring& ColumnName) const;

   // The current size of the column name vector for this result set, which
   // should be identical in size to the column value vector of each row object.
   const COLuint32 countOfColumn() const;

   // Add a column to this result set. Returns the vector index 
   // of the added column name. Also resizes the column vector of any 
   // existing row object members to accomodate this added column.
   const COLuint32 addColumn( const COLstring& ColumnName, DBdataType DataType );

   // Remove a column from the result set by column index. This function
   // removes the column from both the column name vector and its
   // associated column value in each row object.
   void removeColumn( COLuint32 ColumnIndex );

   // Remove a column from the result set by column name. This function
   // removes the column from both the column name vector and its
   // associated column value in each row object.
   void removeColumn( const COLstring& ColumnName );

   // For performance reasons, you should set the number of rows in the 
   // result set before specifying the details of each row.
   //
   // Each row object contains a vector of column values. This vector 
   // of column values is automatically resized to accommodate the current 
   // number of columns (specified by the current number of column names). 
   void resizeRowVector( COLuint32 CountOfRow );

   // Add a row object to this result set. Returns a reference to the added
   // row object. The row's vector of column values is automatically resized 
   // to accommodate the current number of columns (specified by the current 
   // number of column names).
   DBresultSetRow& addRow();

   // Remove a row from the result set by row index.
   void removeRow( COLuint32 RowIndex );

   // The function to access each row object via row index.
   DBresultSetRow& row( COLuint32 RowIndex ) const;

   // The current size of the row vector for this result set.
   const COLuint32 countOfRow() const;

   // Set the total number of result set rows that are available from the
   // database.
   void setCountOfDatabaseRow( COLuint64 CountOfDatabaseRow );
   
   // Retrieve the total number of result rows that are available from
   // the database.
   COLuint64 countOfDatabaseRow() const;

   // Set a value in the result set by specifying row and column indices. 
   // The DBvariant class automatically constructs typed objects from 
   // the supported simple types.
   //
   // Note: NULL result set values are represented by a DB_DATA_TYPE_NOT_DEFINED
   void setValue( COLuint32 RowIndex, COLuint32 ColumnIndex, const DBvariant& Value ) const;

   // Set a value in the result set by specifying a row index and a column 
   // name. The DBvariant class automatically constructs typed objects from 
   // the supported simple types.
   //
   // Note: NULL result set values are represented by a DB_DATA_TYPE_NOT_DEFINED
   void setValue( COLuint32 RowIndex, const COLstring& ColumnName, const DBvariant& Value ) const;
 
   // Retrieve a value from the result set by row and column indices.
   // The type of a returned DBvariant object may be checked with the dataType() 
   // function. Its value may be converted to the required type for usage (look at 
   // the header file DBvariant.h for more details).
   //
   // Note: NULL result set values are represented by DB_DATA_TYPE_NOT_DEFINED.
   const DBvariant& value( COLuint32 RowIndex, COLuint32 ColumnIndex ) const;
   
   // Retrieve a value from the result set by a row index and a column name.
   // The type of a returned DBvariant object may be checked with the baseType() 
   // function. Its value may be converted to the required type for usage (look at 
   // the header file DBvariant.h for more details).
   //
   // Note: NULL result set values are represented by DB_DATA_TYPE_NOT_DEFINED.
   
   const DBvariant& value( COLuint32 RowIndex, const COLstring& ColumnName ) const;

private:
   DBresultSetPrivate* pMember;

   DBresultSet( DBresultSet& Orig ); // not allowed
   DBresultSet& operator=( DBresultSet& Orig ); // not allowed
};

#endif // end of defensive include
