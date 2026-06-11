//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlSelectColumn
//
// Description:
//
// Implementation
//
// Author: George Ma
// Date:   July 6, 2004 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBsqlSelectColumn.h"

DBsqlSelectColumn::DBsqlSelectColumn()
: CastType(DB_DATA_TYPE_NOT_DEFINED)
{
}

DBsqlSelectColumn::~DBsqlSelectColumn()
{
}

void DBsqlSelectColumn::setCastType( DBdataType iCastType )
{
   CastType = iCastType;
}

DBdataType DBsqlSelectColumn::castType() const
{
   return CastType;
}
