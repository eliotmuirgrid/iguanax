//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlCreateTable
//
// Description:
//
// Implementation.
//
// Author: Henry Tran
// Date:   November 25, 2003 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBsqlCreateTable.h"
#include "DBsqlCreateTableColumn.h"

#include <COL/COLarray.h>

class DBsqlCreateTablePrivate
{
public:
   DBsqlCreateTablePrivate() {}
   ~DBsqlCreateTablePrivate() {}
   
   COLstring TableName;
   bool QuoteTableName;
   COLarray< DBsqlCreateTableColumn > ColumnVector;
};


DBsqlCreateTable::DBsqlCreateTable()
{
   pMember = new DBsqlCreateTablePrivate();
}

DBsqlCreateTable::DBsqlCreateTable( const DBsqlCreateTable& Orig )
{
   pMember = new DBsqlCreateTablePrivate(*Orig.pMember);
}

DBsqlCreateTable& DBsqlCreateTable::operator=( const DBsqlCreateTable& Orig )
{
   *pMember = *Orig.pMember; 
   return *this;
}

DBsqlCreateTable::~DBsqlCreateTable()
{
   delete pMember;
}

void DBsqlCreateTable::setTableName
( 
   const COLstring& TableName, 
   bool QuoteName
)
{
   COLPRECONDITION( !TableName.is_null() );
   pMember->TableName = TableName;
   pMember->QuoteTableName = QuoteName;
}

const COLstring& DBsqlCreateTable::tableName() const
{
   return pMember->TableName;
}

bool DBsqlCreateTable::quoteTableName() const
{
   return pMember->QuoteTableName;
}

DBsqlCreateTableColumn& DBsqlCreateTable::addColumn()
{
   return pMember->ColumnVector.push_back();
}

DBsqlCreateTableColumn& DBsqlCreateTable::column( COLuint32 ColumnIndex ) const
{
   return pMember->ColumnVector[ ColumnIndex ];
}

const COLuint32 DBsqlCreateTable::countOfColumn() const
{
   return pMember->ColumnVector.size();
}
