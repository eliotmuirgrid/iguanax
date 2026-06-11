//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlInsert
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

#include "DBsqlInsert.h"
#include "DBsqlInsertColumn.h"
#include "DBsqlSelect.h"

#include <COL/COLarray.h>

class DBsqlInsertPrivate
{
public:
   DBsqlInsertPrivate() 
   : SelectQueryExists(false),
     QuoteTableName(false)
   {}
   ~DBsqlInsertPrivate() {}

   COLstring TableName;
   bool QuoteTableName;
   COLarray< DBsqlInsertColumn > ColumnVector;

   bool SelectQueryExists;
   DBsqlSelect SelectQuery;
};

DBsqlInsert::DBsqlInsert()
{
   pMember = new DBsqlInsertPrivate();
}

DBsqlInsert::DBsqlInsert( const DBsqlInsert& Orig )
{
   pMember = new DBsqlInsertPrivate(*Orig.pMember);   
}

DBsqlInsert& DBsqlInsert::operator=( const DBsqlInsert& Orig )
{
   *pMember = *Orig.pMember;
   return *this;
}

DBsqlInsert::~DBsqlInsert()
{
   delete pMember;
}

void DBsqlInsert::setTableName( const COLstring& TableName, bool QuoteTableName )
{
   COLPRECONDITION( !TableName.is_null() );
   pMember->TableName = TableName;
   pMember->QuoteTableName = QuoteTableName;
}

const COLstring& DBsqlInsert::tableName() const
{
   return pMember->TableName;
}

bool DBsqlInsert::quoteTableName() const
{
   return pMember->QuoteTableName;
}

const COLuint32 DBsqlInsert::addColumn
( 
   const COLstring& ColumnName, 
   const DBvariant& ColumnValue,
   const bool& ValueIsColumnFlag,
   bool QuoteColumnName,
   COLindex       NumberOfRows
)
{
   COLPRECONDITION( !ColumnName.is_null() );

   pMember->ColumnVector.push_back();
   COLuint32 AddedColumnIndex = pMember->ColumnVector.size() - 1;
   pMember->ColumnVector[ AddedColumnIndex ].setName( ColumnName );
   pMember->ColumnVector[ AddedColumnIndex ].setCountOfRows ( NumberOfRows );
   pMember->ColumnVector[ AddedColumnIndex ].setValue( ColumnValue );
   pMember->ColumnVector[ AddedColumnIndex ].setValueIsColumnFlag( ValueIsColumnFlag );
   pMember->ColumnVector[ AddedColumnIndex ].setQuoteName( QuoteColumnName );
   return AddedColumnIndex;
}

void DBsqlInsert::setColumnValue( COLuint32 Col, COLuint32 Row, const DBvariant& Value)
{
   COLASSERT (Row < countOfRows());
   pMember->ColumnVector[ Col ].setValue( Value, Row );
}

bool DBsqlInsert::removeColumn( const COLstring& ColumnName)
{
   for (COLuint32 ColumnIndex = 0; ColumnIndex < pMember->ColumnVector.size(); ColumnIndex++)
   {
      if (pMember->ColumnVector[ColumnIndex].name() == ColumnName)
      {
         pMember->ColumnVector.remove(ColumnIndex);
         return true;
      }
   }
   return false;
}

void DBsqlInsert::resizeColumnVector( COLuint32 Size)
{
   pMember->ColumnVector.resize( Size );
}

void DBsqlInsert::setColumn
( 
   COLuint32 ColumnIndex,
   const COLstring& ColumnName, 
   const DBvariant& ColumnValue,
   const bool& ValueIsColumnFlag,
   bool QuoteColumnName
)
{
   COLPRECONDITION( !ColumnName.is_null() );

   COLPRECONDITION( ColumnIndex < pMember->ColumnVector.size() )
   DBsqlInsertColumn& InsertColumn = pMember->ColumnVector[ ColumnIndex ];
   InsertColumn.setName( ColumnName );
   InsertColumn.setValue( ColumnValue );
   InsertColumn.setValueIsColumnFlag( ValueIsColumnFlag );
   InsertColumn.setQuoteName( QuoteColumnName );
}
   
const COLstring& DBsqlInsert::columnName( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->ColumnVector.size() )
   return pMember->ColumnVector[ ColumnIndex ].name();
}

const DBvariant& DBsqlInsert::columnValue( COLuint32 ColumnIndex, COLuint32 RowIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->ColumnVector.size() )
   return pMember->ColumnVector[ ColumnIndex ].value(RowIndex);
}
   
const bool& DBsqlInsert::columnValueIsColumnFlag( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->ColumnVector.size() )
   return pMember->ColumnVector[ ColumnIndex ].valueIsColumnFlag();
}

bool DBsqlInsert::quoteColumnName( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->ColumnVector.size() )
   return pMember->ColumnVector[ ColumnIndex ].quoteName();
}

const COLuint32 DBsqlInsert::countOfColumn() const
{
   return pMember->ColumnVector.size();
}

void DBsqlInsert::addSelectQuery()
{
   COLPRECONDITION(!selectQueryExists());
   pMember->SelectQueryExists = true;
}
   
DBsqlSelect& DBsqlInsert::selectQuery() const
{
   COLPRECONDITION(selectQueryExists());
   return pMember->SelectQuery;
}

const bool DBsqlInsert::selectQueryExists() const
{
   return pMember->SelectQueryExists;
}

COLindex DBsqlInsert::countOfRows() const
{
   COLindex NumRows(0);
   if (pMember->ColumnVector.size())
   {
      NumRows = pMember->ColumnVector[0].countOfRows();
   }
   return NumRows;
}
