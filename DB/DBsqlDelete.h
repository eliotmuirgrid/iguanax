#ifndef __DB_SQL_DELETE_H__
#define __DB_SQL_DELETE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlDeleteDrop
//
// Description:
//
// SQL command class for DELETE operations (to delete rows from a database 
// table), which consists of a table name and an optional WHERE clause, 
// all encapsulated in an object.
//
// Since this command has the same basic structure as the DROP TABLE command 
// but also has an optional WHERE clause, this class inherits from the 
// DBsqlDropTable class.
//
// Author: Henry Tran
// Date:   November 28, 2003 
//
//---------------------------------------------------------------------------

#include "DBsqlDropTable.h"

class DBsqlDeletePrivate;
class DBsqlWhere;

class DBsqlDelete : public DBsqlDropTable
{
public:
   DBsqlDelete();
   DBsqlDelete( const DBsqlDelete& Orig ); 
   DBsqlDelete& operator=( const DBsqlDelete& Orig ); 
   virtual ~DBsqlDelete();

   virtual DBsqlCommandType commandType() const { return DB_DELETE_CMD; }

   // The following set and get methods for the name of the table to delete 
   // rows from are defined in the parent class, DBsqlDropTable.
   //
   // void setTableName( const COLstring& TableName );
   // const COLstring& tableName() const;

   // Optionally specify which rows to delete through a WHERE clause.
   // If this is not specified, all rows in the table will be deleted.
   DBsqlWhere& addWhereClause();

   void removeWhereClause();

   // Access (and set) the expression for the above WHERE clause through
   // the returned reference.
   DBsqlWhere& whereClause() const;

   // Returns true if a WHERE clause has been added to this DELETE command.
   const bool whereClauseExists() const;

private:
   DBsqlDeletePrivate* pMember;

};

#endif // end of defensive include
