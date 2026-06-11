//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlDropTable
//
// Description:
//
// Implementation
// 
// Author: Henry Tran
// Date:   January 9, 2004 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBsqlDropTable.h"


DBsqlDropTable::DBsqlDropTable()
: QuoteName(false)
{
   // do nothing
}

DBsqlDropTable::~DBsqlDropTable(){}

void DBsqlDropTable::setTableName( const COLstring& iTableName, bool iQuoteName )
{
   COLPRECONDITION( !iTableName.is_null() );
   TableName = iTableName;
   QuoteName = iQuoteName;
}

const COLstring& DBsqlDropTable::tableName() const
{
   return TableName;
}

bool DBsqlDropTable::quoteTableName() const
{
   return QuoteName;
}
