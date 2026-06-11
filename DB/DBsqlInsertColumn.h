#ifndef __DB_SQL_INSERT_COLUMN_H__
#define __DB_SQL_INSERT_COLUMN_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlInsertColumn
//
// Description:
//
// A class to encapsulate information about a single column in an
// INSERT database command. Each column consists of a name, the value to 
// insert into the column, and a flag to specify if the value is another
// column.
//
// If a SELECT query is used in the INSERT command, only the column
// names will be used inside this class.
//
// Encapsulating each column's information will better orgranize the INSERT 
// command structure when it is serialized (i.e. all column information is 
// grouped together). However, when using the DB2 API, it is probably more 
// desirable to specify column information directly on the DBsqlInsert 
// object.
//
// Author: Henry Tran
// Date:   December 1, 2003
//
//---------------------------------------------------------------------------

#include "DBvariant.h"
#include <COL/COLarray.h>

class DBsqlInsertColumn
{
public:
   DBsqlInsertColumn();
   // POD - so shallow copy constructor and assignment operator is fine.
   virtual ~DBsqlInsertColumn();

   void setName( const COLstring& Name );
   const COLstring& name() const;

   void setValue( const DBvariant& Value, COLindex RowIndex = 0);
   const DBvariant& value(COLindex RowIndex = 0) const;

   void setValueIsColumnFlag( const bool& ValueIsColumnFlag );
   const bool& valueIsColumnFlag() const;

   void setQuoteName( bool QuoteName );
   bool quoteName() const;

   bool multiRow () { return MultiRow; }
   void setMultiRow (bool Mr) { MultiRow = Mr; }
   void setCountOfRows (COLindex NumRows) { Values.resize (NumRows); }
   COLindex countOfRows () { return Values.size(); }
private:
   COLstring Name;
   COLarray <DBvariant> Values;
   bool ValueIsColumnFlag;
   bool QuoteName;
   bool MultiRow;
};


#endif // end of defensive include
