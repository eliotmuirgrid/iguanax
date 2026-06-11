#ifndef __DB_SQL_SELECT_COLUMN_H__
#define __DB_SQL_SELECT_COLUMN_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlSelectColumn
//
// Description:
//
// A class to encapsulate information about a single column in a 
// SELECT database query. DBsqlSelectColumn inherits from DBsqlSelectNameAlias.
// Each column consists of a name, an optional, alias, and an optional type cast.
//
// Beware that not all databases support the CAST operation, and even then
// it is supported to varying degrees depending on the database. 
//
// Author: George Ma
// Date:   July 6, 2004 
//
//---------------------------------------------------------------------------

#include "DBminimumInclude.h"
#include "DBsqlSelectNameAlias.h"

class DBsqlSelectColumn :  public DBsqlSelectNameAlias
{
public:
   DBsqlSelectColumn();
   // POD so default copy constructor and assignment operator will work
   virtual ~DBsqlSelectColumn();

   void setCastType( DBdataType CastType );
   DBdataType castType() const;  

private:
   DBdataType CastType;
};

#endif // end of defensive include
