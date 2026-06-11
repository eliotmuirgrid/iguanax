#ifndef __DB_SQL_SELECT_ORDER_BY_H__
#define __DB_SQL_SELECT_ORDER_BY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlSelectOrderBy
//
// Description:
//
// A class to encapsulate information about a single column in a 
// the ORDER BY clause of a SELECT query. Each column consists of a name 
// and an optional descending flag.
//
// The rows in the result set table will be sorted with respect to the values
// in the column specified here. An optional descending flag may be set
// to sort the rows in descending order, otherwise ascending order is assumed.
//
// Encapsulating each order by column's information will better organize the 
// SELECT command structure when it is serialized (i.e. all order by column 
// information is grouped together). However, when using the DB2 API, it is 
// probably more desirable to specify order by column information directly on
// the DBsqlSelect object.
//
// Author: Henry Tran
// Date:   December 1, 2003 
//
//---------------------------------------------------------------------------

class DBsqlSelectOrderBy 
{
public:
   DBsqlSelectOrderBy();
   // POD - so default copy constructor and assignment operator are fine.
   virtual ~DBsqlSelectOrderBy(); // not sure if this needs to be virtual - Eliot

   void setColumnName( const COLstring& ColumnName );
   const COLstring& columnName() const;

   void setQuoteName( bool QuoteName );
   bool quoteName() const;

   void setDescendingFlag( bool DescendingFlag );
   bool descendingFlag() const;

private:
   COLstring ColumnName;
   bool QuoteName;
   bool DescendingFlag;
};

#endif // end of defensive include
