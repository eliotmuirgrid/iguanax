//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlUnionSelect
//
// Description:
//
// Implementation
//
// Author: Henry Tran
// Date:   January 27, 2004
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBsqlSelectUnion.h"
#include "DBsqlSelect.h"
#include "DBsqlSelectOrderBy.h"

#include <COL/COLarray.h>

class DBsqlSelectUnionPrivate
{
public:
   DBsqlSelectUnionPrivate()
   : AllFlag(false)
   {}
   ~DBsqlSelectUnionPrivate() {}

   bool AllFlag;
   COLarray< DBsqlSelect > SelectVector;
   COLarray< DBsqlSelectOrderBy > OrderByVector;
};


DBsqlSelectUnion::DBsqlSelectUnion()
{
   pMember = new DBsqlSelectUnionPrivate();
}

DBsqlSelectUnion::DBsqlSelectUnion( const DBsqlSelectUnion& Orig )
{
   pMember = new DBsqlSelectUnionPrivate(*Orig.pMember);
}

DBsqlSelectUnion& DBsqlSelectUnion::operator=( const DBsqlSelectUnion& Orig )
{
   *pMember = *Orig.pMember;
   return *this;
}

DBsqlSelectUnion::~DBsqlSelectUnion()
{
   delete pMember;
}

void DBsqlSelectUnion::setAllFlag( bool NewValue )
{
   pMember->AllFlag = NewValue;
}

const bool& DBsqlSelectUnion::allFlag() const
{
   return pMember->AllFlag;
}

const COLuint32 DBsqlSelectUnion::addSelectQuery()
{
   pMember->SelectVector.push_back();
   return pMember->SelectVector.size() - 1;
}

DBsqlSelect& DBsqlSelectUnion::selectQuery( COLuint32 SelectIndex ) const
{
   COLPRECONDITION( SelectIndex < pMember->SelectVector.size() )
   return pMember->SelectVector[ SelectIndex ];
}

const COLuint32 DBsqlSelectUnion::countOfSelectQuery() const
{
   return pMember->SelectVector.size();
}

DBsqlSelectOrderBy& DBsqlSelectUnion::addOrderByColumn()
{
   return pMember->OrderByVector.push_back();
}

DBsqlSelectOrderBy& DBsqlSelectUnion::orderByColumn( COLuint32 OrderByIndex ) const
{
   COLPRECONDITION( OrderByIndex < pMember->OrderByVector.size() )
   return pMember->OrderByVector[OrderByIndex];
}

COLuint32 DBsqlSelectUnion::countOfOrderByColumn() const
{
   return pMember->OrderByVector.size();
}
