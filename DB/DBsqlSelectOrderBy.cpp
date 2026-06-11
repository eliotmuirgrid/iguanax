//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlSelectOrderBy
//
// Description:
//
// Implementation
//
// Author: Henry Tran
// Date:   December 1, 2003 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBsqlSelectOrderBy.h"

DBsqlSelectOrderBy::DBsqlSelectOrderBy()
: QuoteName(false),
  DescendingFlag(false)
{
}

DBsqlSelectOrderBy::~DBsqlSelectOrderBy()
{
}

void DBsqlSelectOrderBy::setColumnName( const COLstring& NewValue )
{
   COLPRECONDITION( !NewValue.is_null() );
   ColumnName = NewValue;
}

const COLstring& DBsqlSelectOrderBy::columnName() const
{
   return ColumnName;
}

void DBsqlSelectOrderBy::setQuoteName( bool NewValue )
{
   QuoteName = NewValue;
}

bool DBsqlSelectOrderBy::quoteName() const
{
   return QuoteName;
}

void DBsqlSelectOrderBy::setDescendingFlag( bool NewValue )
{
   DescendingFlag = NewValue;
}

bool DBsqlSelectOrderBy::descendingFlag() const
{
   return DescendingFlag;
}
