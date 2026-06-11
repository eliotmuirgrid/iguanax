//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlInsertColumn
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

#include "DBsqlInsertColumn.h"

DBsqlInsertColumn::DBsqlInsertColumn()
: QuoteName(false),
  ValueIsColumnFlag(false),
  Values(1),
  MultiRow (false)
{
}

DBsqlInsertColumn::~DBsqlInsertColumn()
{
}

void DBsqlInsertColumn::setName( const COLstring& iName )
{
   COLPRECONDITION( !iName.is_null() );
   Name = iName;
}

const COLstring& DBsqlInsertColumn::name() const
{
   return Name;
}

void DBsqlInsertColumn::setValue( const DBvariant& iValue, COLindex RowIndex )
{
   COLASSERT (RowIndex == 0 || RowIndex < Values.size());
   Values[RowIndex] = iValue;
}

const DBvariant& DBsqlInsertColumn::value(COLindex RowIndex) const
{
   COLASSERT (RowIndex == 0 || RowIndex < Values.size());
   return Values[RowIndex];
}

void DBsqlInsertColumn::setValueIsColumnFlag( const bool& iValueIsColumnFlag )
{
   ValueIsColumnFlag = iValueIsColumnFlag;
}

const bool& DBsqlInsertColumn::valueIsColumnFlag() const
{
   return ValueIsColumnFlag;
}

void DBsqlInsertColumn::setQuoteName( bool iQuoteName )
{
   QuoteName = iQuoteName;
}

bool DBsqlInsertColumn::quoteName() const
{
   return QuoteName;
}
