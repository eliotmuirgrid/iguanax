#ifndef __DB_SQL_SELECT_H__
#define __DB_SQL_SELECT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlSelect
//
// Description:
//
// SQL command class for SELECT queries (to retrieve rows from database 
// tables), which consists of table names, column names, and other parameters, 
// all encapsulated in an object.
//
// Author: Henry Tran
// Date:   November 25, 2003 
//
//---------------------------------------------------------------------------

#include "DBminimumInclude.h"
#include "DBsqlCommand.h"

class DBsqlSelectJoin;
class DBsqlSelectOrderBy;
class DBsqlSelectPrivate;
class DBsqlWhere;

class DBsqlSelect : public DBsqlCommand
{
public:
   DBsqlSelect();
   DBsqlSelect( const DBsqlSelect& Orig ); 
   DBsqlSelect& operator=( const DBsqlSelect& Orig ); 
   virtual ~DBsqlSelect();

   virtual DBsqlCommandType commandType() const { return DB_SELECT_CMD; }

   // Set this flag to true to exclude duplicate row values in the result 
   // set. The default value for this flag is false.
   void setDistinctFlag( bool DistinctFlag );

   // Retrieve the distinct flag for this SELECT query.
   const bool& distinctFlag() const;

   // Add a name of a table to select rows from.
   const COLuint32 addTable( const COLstring& TableName, 
                             const COLstring& TableAlias = "", 
                             bool QuoteName = false, 
                             bool QuoteAlias = false );
   
   // Retrieve a name of a table to select rows from by vector index.
   const COLstring& tableName( COLuint32 TableIndex = 0 ) const;
   bool quoteTableName( COLuint32 TableIndex = 0 ) const;

   const COLstring& tableAlias( COLuint32 TableIndex = 0 ) const;
   bool quoteTableAlias( COLuint32 TableIndex = 0 ) const;

   // Returns the current size of the table name vector for this SELECT query.
   const COLuint32 countOfTableName() const;

   // Add a column to select from the database table. If multiple tables
   // are specified, the table name should prefix the column name. Optionally
   // specify an alias for the column (so that the column will be named differently
   // in the result set). Note that SQL functions may be used as the "ColumnName". 
   // This function returns the index of the added column.
   const COLuint32 addColumn( const COLstring& ColumnName, 
                              const COLstring& ColumnAlias = "", 
                              DBdataType ColumnCastType = DB_DATA_TYPE_NOT_DEFINED,
                              bool QuoteName = false,
                              bool QuoteAlias = false );

   
   // Clears all columns for the database table, from the above add.
   void clearColumns();
   
   // Get a column name to query data from by vector index.
   const COLstring& columnName( COLuint32 ColumnIndex ) const;
   bool quoteColumnName( COLuint32 ColumnIndex ) const;

   // Retrieve the alias for a corresponding column by vector index.
   const COLstring& columnAlias( COLuint32 ColumnIndex ) const; 
   bool quoteColumnAlias( COLuint32 ColumnIndex ) const;

   // Retrieve the cast type for a corresponding column by vector index.
   DBdataType columnCastType( COLuint32 ColumnIndex ) const; 
  
   
   // The current size of the column vector for this select query. Note that
   // the size of the column name and column alias vectors are identical.
   const COLuint32 countOfColumn() const;

   
   // Optionally add a WHERE clause to specify which rows to select from the
   // database table(s). If this is not specified, all rows in the table(s) 
   // will be retrieved.
   DBsqlWhere& addWhereClause();

   // Access (and set) the expression for the above WHERE clause through
   // the returned reference.
   DBsqlWhere& whereClause() const;

   // Returns true if a WHERE clause has been added to this SELECT query.
   const bool whereClauseExists() const;

   DBsqlSelectJoin& addJoinClause();
   DBsqlSelectJoin& joinClause() const;
   const bool joinClauseExists() const;

   // Optionally add a group by column to group result set rows together 
   // according to the result set values in this column. Returns the index of the
   // added group by column name.
   const COLuint32 addGroupByColumnName( const COLstring& ColumnName, 
                                         bool QuoteName = false );

   // Retrieve a group by column name for this SELECT query.
   const COLstring& groupByColumnName( COLuint32 ColumnIndex ) const;
   bool quoteGroupByColumnName( COLuint32 ColumnIndex ) const;

   // Returns the current size of the group by vector for this SELECT query.
   const COLuint32 countOfGroupByColumnName() const;

   // Optionally add a HAVING clause to specify which groups to select from
   // the database table(s). This is used in conjunction with the "Group By" 
   // columns added above. If this is not specified, all groups in the table 
   // will be retrieved.
   void addHavingClause();

   // Access (and set) the expression for the above HAVING clause through
   // the returned reference.
   DBsqlWhere& havingClause() const;

   // Returns true if a HAVING clause has been added to this SELECT query.
   const bool havingClauseExists() const;

   // Optionally add an order by column to sort the result set rows based 
   // on this column. May also set an optional "descending" flag to sort 
   // the rows in descending order with respect to the values in this column 
   // (by default ascending order is assumed).
   const COLuint32 addOrderByColumn( const COLstring ColumnName, 
                                     bool DecendingFlag = false,
                                     bool QuoteName = false );

   DBsqlSelectOrderBy& orderByColumn( COLuint32 ColumnIndex ) const;

   // Retrieve an order by column for this SELECT query by vector index.
   //
   const COLstring& orderByColumnName( COLuint32 ColumnIndex ) const;
   bool quoteOrderByColumnName( COLuint32 ColumnIndex ) const;

   // Special for Iguana2 for removing ORDER BY clause.
   void clearOrderByClause();

   // Set the descending flag of an order by column by vector index.
   void setOrderByDescendingFlag( COLuint32 ColumnIndex, bool DescendingFlag );

   // Retrieve the descending flag for a corresponding order by column
   // by vector index.
   bool orderByDescendingFlag( COLuint32 ColumnIndex ) const;

   // Returns the current size of the order by column vector for 
   // this select command.
   const COLuint32 countOfOrderByColumn() const;

private:
   DBsqlSelectPrivate* pMember;
};

#endif // end of defensive include
