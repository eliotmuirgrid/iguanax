//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlCreateTableColumnForeignKey
//
// Description:
//
// Implementation
//
// Author: Henry Tran
// Date:   November 25, 2003 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBsqlCreateTableColumnForeignKey.h"

class DBsqlCreateTableColumnForeignKeyPrivate
{
public:
   DBsqlCreateTableColumnForeignKeyPrivate() 
   : QuoteTableName(false),
     QuoteColumnName(false),
     OnDeleteAction(DBsqlCreateTableColumnForeignKey::RESTRICT),
     OnUpdateAction(DBsqlCreateTableColumnForeignKey::RESTRICT)   
   {}
   ~DBsqlCreateTableColumnForeignKeyPrivate() {}
   
   COLstring TableName;
   bool QuoteTableName;
   COLstring ColumnName;
   bool QuoteColumnName;
   DBsqlCreateTableColumnForeignKey::eAction OnDeleteAction;
   DBsqlCreateTableColumnForeignKey::eAction OnUpdateAction;
};

DBsqlCreateTableColumnForeignKey::DBsqlCreateTableColumnForeignKey()
{
   pMember = new DBsqlCreateTableColumnForeignKeyPrivate();
}

DBsqlCreateTableColumnForeignKey::DBsqlCreateTableColumnForeignKey( const DBsqlCreateTableColumnForeignKey& Orig )
{
   pMember = new DBsqlCreateTableColumnForeignKeyPrivate(*Orig.pMember);
}

DBsqlCreateTableColumnForeignKey& DBsqlCreateTableColumnForeignKey::operator=( const DBsqlCreateTableColumnForeignKey& Orig )
{
   *pMember = *Orig.pMember;
   return *this;
}

DBsqlCreateTableColumnForeignKey::~DBsqlCreateTableColumnForeignKey()
{
   delete pMember;
}

void DBsqlCreateTableColumnForeignKey::setTableName( const COLstring& TableName )
{
   COLPRECONDITION( !TableName.is_null() );
   pMember->TableName = TableName;
}

const COLstring& DBsqlCreateTableColumnForeignKey::tableName() const
{
   return pMember->TableName;
}

void DBsqlCreateTableColumnForeignKey::setQuoteTableName( bool QuoteName )
{
   pMember->QuoteTableName = QuoteName;
}

bool DBsqlCreateTableColumnForeignKey::quoteTableName() const
{
   return pMember->QuoteTableName;
}

void DBsqlCreateTableColumnForeignKey::setColumnName( const COLstring& ColumnName )
{
   COLPRECONDITION( !ColumnName.is_null() );
   pMember->ColumnName = ColumnName;
}

const COLstring& DBsqlCreateTableColumnForeignKey::columnName() const
{
   return pMember->ColumnName;
}

void DBsqlCreateTableColumnForeignKey::setQuoteColumnName( bool QuoteName )
{
   pMember->QuoteColumnName = QuoteName;
}

bool DBsqlCreateTableColumnForeignKey::quoteColumnName() const
{
   return pMember->QuoteColumnName;
}

void DBsqlCreateTableColumnForeignKey::setOnDeleteAction( eAction Action )
{
   pMember->OnDeleteAction = Action;
}

const DBsqlCreateTableColumnForeignKey::eAction DBsqlCreateTableColumnForeignKey::onDeleteAction() const
{
   return pMember->OnDeleteAction;
}

void DBsqlCreateTableColumnForeignKey::setOnUpdateAction( eAction Action )
{
   pMember->OnUpdateAction = Action;
}

const DBsqlCreateTableColumnForeignKey::eAction DBsqlCreateTableColumnForeignKey::onUpdateAction() const
{
   return pMember->OnUpdateAction;
}
