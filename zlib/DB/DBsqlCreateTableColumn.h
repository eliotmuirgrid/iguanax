#ifndef __DB_SQL_CREATE_TABLE_COLUMN_H__
#define __DB_SQL_CREATE_TABLE_COLUMN_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlCreateTableColumn
//
// Description:
//
// A class to encapsulate information about a single column in a 
// CREATE TABLE command. Column parameters that mey be set include name, 
// data type, default value (optional), length (optional), required flag
// (optional), and primary key flag (optional).
//
// Encapsulating each column facilitates extensibility because if more 
// column parameters are added, it will not affect code that already
// utilizes this class.
//
// Author: Henry Tran
// Date:   December 1, 2003 
//
// 
//---------------------------------------------------------------------------

#include "DBsqlCreateTableColumnForeignKey.h"

#include "DBminimumInclude.h"

class DBvariant;

class DBsqlCreateTableColumnPrivate;

class DBsqlCreateTableColumn
{
public:
   DBsqlCreateTableColumn();
   DBsqlCreateTableColumn(const COLstring& Name, DBdataType DataType);
   DBsqlCreateTableColumn( const DBsqlCreateTableColumn& Orig ); 
   DBsqlCreateTableColumn& operator=( const DBsqlCreateTableColumn& Orig ); 
   virtual ~DBsqlCreateTableColumn();

   // Set the name for this column.
   void setName( const COLstring& Name );

   // Retrieve the name of this column.
   const COLstring& name() const;

   void setQuoteName( bool QuoteName );
   bool quoteName() const;

   // Set the data type for this column. See "DB2/DBminimumInclude.h" for a list
   // of supported data types.
   void setDataType( DBdataType DataType );

   // Retrieve data type for this column.
   const DBdataType dataType() const;
   COLstring dataTypeAsString() const;

   // Optionally specify a default value for this column. By default, the 
   // column will have no default value.
   void setDefaultValue( const DBvariant& DefaultValue );

   // Retrieve the default value for this column. Calling 
   // dataType() != DB_DATA_TYPE_NOT_DEFINED
   // with the returned reference checks if a default value exists.
   const DBvariant& defaultValue() const;

   // Optionally set the length of this column. By default this length is
   // zero, which implies this column will be created with a default length
   // (actual length varies depending upon the specified data type).
   void setLength( const COLuint32 Length );

   // Retrieve the length of this column.
   const COLuint32& length() const;

   // Optionally specify that this column is required (i.e. no NULL values 
   // are allowed in this column). By default, this flag is false, meaning 
   // that the column is not required.
   void setRequiredFlag( bool RequiredFlag );
   
   // Retrieve the required flag for this column.
   const bool& requiredFlag() const;

   // Optionally specify that this column is the primary key or is part of
   // the primary key (i.e. the primary key consists of more than one column)
   // for this table. By default, this flag is false. meaning that the column 
   // is not a primary key.
   void setPrimaryKeyFlag( bool PrimaryKeyFlag );

   // Retrieve the primary key flag for this column.
   const bool& primaryKeyFlag() const;

   // Optionally add a foreign key constraint to link this column with a
   // column in a foreign table. Foreign keys may be used to prevent 
   // deletion/updating of linked data in the foreign table (i.e. the rows 
   // that would be able to be affected ain the foreign table are the ones 
   // that have the same values for the linked columns in the local table), 
   // or even cascade deletion/updating of a row in the foreign table 
   // to the row(s) in local table that contain linked data.
   void addForeignKey( const COLstring& ForeignTableName, 
                       const COLstring& ForeignColumnName,
                       DBsqlCreateTableColumnForeignKey::eAction OnDeleteAction,
                       DBsqlCreateTableColumnForeignKey::eAction OnUpdateAction,
                       bool QuoteForeignTableName = false,
                       bool QuoteForeignColumnName = false );

   // Retrieve the name of the foreign table for the foreign key constraint.
   const COLstring& foreignTableName() const;

   bool quoteForeignTableName() const;

   // Retrieve the name of the foreign column for the foreign key constraint.
   const COLstring& foreignColumnName() const;

   bool quoteForeignColumnName() const;

   // Retrieve the on delete action for the specified foreign key constraint.
   // TODO - change eAction to Action
   const DBsqlCreateTableColumnForeignKey::eAction foreignKeyOnDeleteAction() const;

   // Retrieve the on delete action for the specified foreign key constraint.
   const DBsqlCreateTableColumnForeignKey::eAction foreignKeyOnUpdateAction() const;

   const bool foreignKeyExists() const;

   void removeForeignKey();

private:
   DBsqlCreateTableColumnPrivate* pMember;

};

#endif // end of defensive include
