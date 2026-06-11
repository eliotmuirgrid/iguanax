#ifndef __DB_SQL_CREATE_TABLE_COLUMN_FOREIGN_KEY_H__
#define __DB_SQL_CREATE_TABLE_COLUMN_FOREIGN_KEY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlCreateTableColumnForeignKey
//
// Description:
//
// A class to encapsulate foreign key information about a single column 
// in a CREATE TABLE command. It is a member of DBsqlCreateTableColumn. 
// Foreign key parameters that mey be specified include the foreign table 
// name, foreign column name, on delete action (optional), and on update 
// action (optional).
//
// Encapsulating each column's foreign key information will better orgranize 
// the CREATE TABLE command structure when it is serialized (i.e. all foreign
// key information is grouped together). However, when using the DB2 API, it 
// is probably more desirable to specify foreign key constraints directly on
// the DBsqlCreateTableColumn object.
//
// Author: Henry Tran
// Date:   November 25, 2003 
//
//---------------------------------------------------------------------------

class DBsqlCreateTableColumnForeignKeyPrivate;

class DBsqlCreateTableColumnForeignKey 
{
public:
   DBsqlCreateTableColumnForeignKey();
   DBsqlCreateTableColumnForeignKey( const DBsqlCreateTableColumnForeignKey& Orig ); 
   DBsqlCreateTableColumnForeignKey& operator=( const DBsqlCreateTableColumnForeignKey& Orig ); 
   virtual ~DBsqlCreateTableColumnForeignKey();

   void setTableName( const COLstring& TableName );

   // Retrieve the name of the foreign table for this foreign key constraint.
   const COLstring& tableName() const;

   void setQuoteTableName( bool QuoteName );
   bool quoteTableName() const;

   void setColumnName( const COLstring& ColumnName );

   // Retrieve the name of the foreign column for this foreign key constraint.
   const COLstring& columnName() const;

   void setQuoteColumnName( bool QuoteName );
   bool quoteColumnName() const;

   // These are the possible "actions" that the database will perform when
   // linked data is deleted or updated in the foreign key table.
   // TODO change eAction to Action
   enum eAction
   {
      RESTRICT, // Does not allow linked data to be deleted/updated in the 
                // foreign table (throws an error instead). This is the default 
                // action for both deleting and updating operations.
      
      CASCADE   // Delete/update the corresponding row(s) in the local table. 
   };

   void setOnDeleteAction( eAction Action );
   
   // Retrieve the on delete action for the specified foreign key constraint.
   const eAction onDeleteAction() const;
 
    // Set update action
   void setOnUpdateAction( eAction Action );

   // Retrieve the on delete action for the specified foreign key constraint.
   const eAction onUpdateAction() const;

private:
   DBsqlCreateTableColumnForeignKeyPrivate* pMember;
};

#endif // end of defensive include
