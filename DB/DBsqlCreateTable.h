#ifndef __DB_SQL_CREATE_TABLE_H__
#define __DB_SQL_CREATE_TABLE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlCreateTable
//
// Description:
//
// SQL command class for CREATE TABLE operations, which consists of a table 
// name and a vector of column information.
//
// Author: Henry Tran
// Date:   November 25, 2003 
//
//---------------------------------------------------------------------------

#include "DBsqlCommand.h"

class DBsqlCreateTableColumn;
class DBsqlCreateTablePrivate;

class DBsqlCreateTable :  public DBsqlCommand
{
public:
   DBsqlCreateTable();
   DBsqlCreateTable( const DBsqlCreateTable& Orig ); 
   DBsqlCreateTable& operator=( const DBsqlCreateTable& Orig ); 
   virtual ~DBsqlCreateTable();

   virtual DBsqlCommandType commandType() const { return DB_CREATE_TABLE_CMD; }

   // Set the name of the table that to be created.
   void setTableName( const COLstring& TableName, bool QuoteName = false );

   bool quoteTableName() const;
   
   // Retrieve the name of the table to be created.
   const COLstring& tableName() const;

   // Add a column (object) to create in the table. Each column object 
   // consists of a name, the column's data type, and other column parameters. 
   // Returns the vector index of the added column object.
   DBsqlCreateTableColumn& addColumn();

   // The accessor method for each column object in the table. The column name,
   // data type, and other parameters may be set/accessed though the returned 
   // reference.
   DBsqlCreateTableColumn& column( COLuint32 ColumnIndex ) const;

   // The current size of the column vector for this table.
   const COLuint32 countOfColumn() const;
private:
   DBsqlCreateTablePrivate* pMember;

};

#endif // end of defensive include
