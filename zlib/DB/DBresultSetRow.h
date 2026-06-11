#ifndef __DB_RESULT_SET_ROW_H__
#define __DB_RESULT_SET_ROW_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBresultSetRow
//
// Description:
//
// A class to encapsulate a single row of a result set from a 
// database query. Each row contains a vector of column values.
//
// A vector of these DBresultSetRow objects is contained in every 
// DBresultSet object.
//
// Author: Henry Tran
// Date:   December 2, 2003 
//
//---------------------------------------------------------------------------


class DBresultSetRowPrivate;
class DBvariant;

class DBresultSetRow
{
public:
   DBresultSetRow();
   DBresultSetRow( const DBresultSetRow& Orig );
   DBresultSetRow& operator=( const DBresultSetRow& Orig ); 
   virtual ~DBresultSetRow();

   // Set a column value in this row by column vector index. The DBvariant
   // class automatically constructs typed objects from the supported simple 
   // types.
   //
   // Note: NULL result set values are represented by a DB_DATA_TYPE_NOT_DEFINED
   void setColumnValue( COLuint32 ColumnIndex, const DBvariant& ColumnValue );

   // Retrieve a single value in this row by column vector index. The type of
   // this DBvariant object may be checked with the dataType() function. Its 
   // value may be converted to the required type for usage (look at the header 
   // file DBvariant.h for more details).
   //
   // Note: NULL result set values are represented by a DB_DATA_TYPE_NOT_DEFINED
   const DBvariant& columnValue( COLuint32 ColumnIndex ) const;

   // The current size of the column value vector for this row.
   COLuint32 countOfColumnValue() const;

   // Removes a value from the column vector of this row object. Generally
   // should not be called directly, but instead, removing a column from
   // DBresultSet calls this function.
   void removeColumnValue( COLuint32 ColumnIndex );

   // Resizes the column value vector to accommodate a specified number
   // of column values. Also should not be called directly, but instead, 
   // adding a row object to DBresultSet calls this function.
   void resizeColumnValueVector( COLuint32 ColumnSize );
private:
   DBresultSetRowPrivate* pMember;   
};

#endif // end of defensive include
