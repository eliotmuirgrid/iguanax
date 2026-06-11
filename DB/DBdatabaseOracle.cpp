//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBdatabaseOracle
//
// Description:
//
// Implementation.
//
// Author:   Henry Tran
// Date:     March 2005 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBdatabaseOracle.h"
#include "DBsqlCreateTable.h"
#include "DBsqlCreateTableColumn.h"

#include <COL/COLdateTime.h>

static const COLstring DBdoubleType("DOUBLE PRECISION");
const COLstring& DBdatabaseOracle::dbDoubleType()
{ 
   return DBdoubleType;
}

static const COLstring DBdateTimeType("DATE");
const COLstring& DBdatabaseOracle::dbDateTimeType()
{
   return DBdateTimeType;
}

static const COLstring DBtextType("CLOB");
const COLstring& DBdatabaseOracle::dbTextType()
{
   return DBtextType;
}

void DBdatabaseOracle::streamCreateTableForeignKeySuffix
( 
   COLostream& Stream, 
   const DBsqlCreateTable& SqlCreateTableCommand,
   COLuint32 ColumnIndex 
)
{
   if ( SqlCreateTableCommand.column( ColumnIndex ).foreignKeyOnDeleteAction() == DBsqlCreateTableColumnForeignKey::CASCADE  )
   {
      Stream << COL_T(" ON DELETE CASCADE ");
   }
}

void DBdatabaseOracle::streamDateTime( COLostream& Stream, const COLdateTime& DateTime )
{
   if ( !DateTime.isNull() )
   {
      Stream << COL_T("TO_DATE('") << DateTime.year() << '-'
             << DateTime.month() << '-'
             << DateTime.day() << ' '
             << DateTime.hour() << ':'
             << DateTime.minute() << ':'
             << DateTime.second() << COL_T("',"
             "'YYYY-MM-DD HH24:MI:SS')");
   }
   else
   {
      streamNullValue( Stream );
   }
}

void DBdatabaseOracle::streamNullValue( COLostream& Stream )
{
   Stream << COL_T("''");
}
