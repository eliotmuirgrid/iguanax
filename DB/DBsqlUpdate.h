#ifndef __DB_SQL_UPDATE_H__
#define __DB_SQL_UPDATE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlUpdate
//
// Description:
//
// SQL command class for UPDATE operations (to update a row [or more] of a 
// database table), which consists of a table name, a vector of column 
// information, and an optional WHERE clause to specify the particular 
// rows to update, all encapsulated in an object.
//
// Since this command has the same basic structure as the INSERT command 
// but also has an optional WHERE clause, this class inherits from the 
// DBsqlInsert class. However, because SELECT queries inside UPDATE commands
// are not widely supported, SELECT queries added to UPDATE commands in this
// class will be ignored.
//
// Author: Henry Tran
// Date:   November 25, 2003 
//
//---------------------------------------------------------------------------

#include "DBsqlInsert.h"

class DBsqlUpdatePrivate;
class DBsqlWhere;

class DBsqlUpdate : public DBsqlInsert
{
public:
   DBsqlUpdate();
   DBsqlUpdate( const DBsqlUpdate& Orig ); 
   DBsqlUpdate& operator=( const DBsqlUpdate& Orig ); 
   virtual ~DBsqlUpdate();

   virtual DBsqlCommandType commandType() const { return CommandType; }

   // Switch type of operation to INSERT/UPDATE
   void setMakeIntoInsert();
   void setMakeIntoUpdate();
   
   // The following methods to set and get the table name and column
   // name/value pairs are defined in the parent class, DBsqlInsert.
   //
   // void setTableName( const COLstring& TableName );
   // const COLstring& tableName() const;
   //
   // const COLuint32 addColumn( const COLstring& ColumnName, const DBvariant& Value = NULL );
   // const COLstring& columnName( const COLuint32& ColumnIndex ) const;
   // const DBvariant& columnValue( const COLuint32& ColumnIndex ) const;
   // const COLuint32 countOfColumn() const;

   // Optionally add a WHERE clause to specify which rows to update.
   // If this is not specified, all rows in the table will be updated
   // with the specified values in the above column vector.
   DBsqlWhere& addWhereClause();

    // Access (and set) the expression for the above WHERE clause through
    // the returned reference.
   DBsqlWhere& whereClause() const;

   // Returns true if a WHERE clause has been added to this UPDATE command.
   const bool whereClauseExists() const;

private:
   DBsqlCommandType CommandType;
   DBsqlUpdatePrivate* pMember;
};

#endif // end of defensive include
