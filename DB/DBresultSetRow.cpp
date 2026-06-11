//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBresultSetRow
//
// Description:
//
// Implementation.
//
// Author: Henry Tran
// Date:   December 2, 2003 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBresultSetRow.h"
#include "DBvariant.h"

#include <COL/COLarray.h>

class DBresultSetRowPrivate
{
public:
   DBresultSetRowPrivate() {}
   ~DBresultSetRowPrivate() {}

   COLarray< DBvariant > ColumnVector;
};

DBresultSetRow::DBresultSetRow()
{
   pMember = new DBresultSetRowPrivate();
}

DBresultSetRow::DBresultSetRow(const DBresultSetRow& Orig)
{
   pMember = new DBresultSetRowPrivate(*Orig.pMember);
}

DBresultSetRow& DBresultSetRow::operator=(const DBresultSetRow& Orig)
{
   *pMember = *Orig.pMember;
   return *this;
}

DBresultSetRow::~DBresultSetRow()
{
   delete pMember;
}

void DBresultSetRow::setColumnValue( COLuint32 ColumnIndex, const DBvariant& ColumnValue )
{
   COLPRECONDITION( ColumnIndex < pMember->ColumnVector.size() )
   pMember->ColumnVector[ColumnIndex] = ColumnValue;
}

const DBvariant& DBresultSetRow::columnValue( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->ColumnVector.size() )
   return pMember->ColumnVector[ColumnIndex];
}

void DBresultSetRow::removeColumnValue( COLuint32 ColumnIndex )
{
   COLPRECONDITION( ColumnIndex < pMember->ColumnVector.size() )
   pMember->ColumnVector.remove( ColumnIndex );
}

COLuint32 DBresultSetRow::countOfColumnValue() const
{
   return pMember->ColumnVector.size();
}

void DBresultSetRow::resizeColumnValueVector( COLuint32 ColumnSize )
{
   pMember->ColumnVector.resize( ColumnSize );
}
