//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlSelectNameAlias
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

#include "DBsqlSelectNameAlias.h"

DBsqlSelectNameAlias::DBsqlSelectNameAlias()
: QuoteName(false),
  QuoteAlias(false)
{
}

DBsqlSelectNameAlias::~DBsqlSelectNameAlias()
{
}

void DBsqlSelectNameAlias::setName( const COLstring& iName )
{
   COLPRECONDITION( !iName.is_null() );
   Name = iName;
}

const COLstring& DBsqlSelectNameAlias::name() const
{
   return Name;
}

void DBsqlSelectNameAlias::setQuoteName( bool iQuoteFlag )
{
   QuoteName = iQuoteFlag;
}

bool DBsqlSelectNameAlias::quoteName() const
{
   return QuoteName;
}

void DBsqlSelectNameAlias::setAlias( const COLstring& iAlias )
{
   Alias = iAlias;
}

const COLstring& DBsqlSelectNameAlias::alias() const
{
   return Alias;
}

void DBsqlSelectNameAlias::setQuoteAlias( bool QuoteFlag )
{
   QuoteAlias = QuoteFlag;
}

bool DBsqlSelectNameAlias::quoteAlias() const
{
   return QuoteAlias;
}
