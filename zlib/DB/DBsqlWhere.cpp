//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlWhere
//
// Description:
//
// Implementation
//
// Author: Henry Tran
// Date:   November 27, 2003 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBsqlWhere.h"
#include "DBsqlWhereItem.h"

#include <COL/COLarray.h>

class DBsqlWherePrivate
{
public:
   DBsqlWherePrivate() 
   : NegateFlag(false) 
   {}
   ~DBsqlWherePrivate() {}
   
   bool  NegateFlag;
   COLarray< DBsqlWhereItem > ItemVector;
};

DBsqlWhere::DBsqlWhere()
{
   pMember = new DBsqlWherePrivate();
}

DBsqlWhere::DBsqlWhere( const DBsqlWhere& Orig )
{
   pMember = new DBsqlWherePrivate(*Orig.pMember);
}

DBsqlWhere& DBsqlWhere::operator=( const DBsqlWhere& Orig )
{
   *pMember = *Orig.pMember;
   return *this;
}

DBsqlWhere::~DBsqlWhere()
{
   delete pMember;
}

void DBsqlWhere::setNegateFlag( bool NegateFlag )
{
   pMember->NegateFlag = NegateFlag;
}

bool DBsqlWhere::negateFlag() const
{
   return pMember->NegateFlag;
}

DBsqlWhereItem& DBsqlWhere::addItem()
{
   DBsqlWhereItem& WhereItem = pMember->ItemVector.push_back();
   return WhereItem;
}
   
DBsqlWhereItem& DBsqlWhere::item( COLuint32 ItemIndex ) const
{
   COLPRECONDITION( ItemIndex < pMember->ItemVector.size() )
   return pMember->ItemVector[ ItemIndex ];
}

COLuint32 DBsqlWhere::countOfItem() const
{
   return pMember->ItemVector.size();
}
