#ifndef __DB_SQL_DROP_TABLE_H__
#define __DB_SQL_DROP_TABLE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlDropTable
//
// Description:
//
// SQL command class for DROP TABLE operations (to drop an entire table from
// a database), which consists of only the name of the table to drop. 
//
// Author: Henry Tran
// Date:   January 9, 2004 
//
//---------------------------------------------------------------------------

#include "DBsqlCommand.h"

class DBsqlDropTable :  public DBsqlCommand
{
public:
   DBsqlDropTable();
   // POD - so default copy constructor and assignment operators will work
   virtual ~DBsqlDropTable();

   virtual DBsqlCommandType commandType() const { return DB_DROP_CMD; }

   // Set the name of the table to drop from the database.
   void setTableName( const COLstring& TableName, bool QuoteName = false );
   bool quoteTableName() const;

   // Retrieve the name of the table that will be dropped from the database.
   const COLstring& tableName() const; 

private:
   COLstring TableName;
   bool QuoteName;
};

#endif // end of defensive include

