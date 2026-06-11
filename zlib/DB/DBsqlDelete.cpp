//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlDelete
//
// Description:
//
// Implementation
//
// Author: Henry Tran
// Date:   November 28, 2003 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBsqlDelete.h"
#include "DBsqlWhere.h"

class DBsqlDeletePrivate
{
public:
   DBsqlDeletePrivate() 
   : HasWhereClause(false)
   {}

   ~DBsqlDeletePrivate() {}
   
   bool HasWhereClause;
   DBsqlWhere WhereClause;
};

DBsqlDelete::DBsqlDelete()
{
   pMember = new DBsqlDeletePrivate();
}

DBsqlDelete::DBsqlDelete( const DBsqlDelete& Orig )
{
   pMember = new DBsqlDeletePrivate(*Orig.pMember);
   setTableName(Orig.tableName(), Orig.quoteTableName());
}

DBsqlDelete& DBsqlDelete::operator=( const DBsqlDelete& Orig )
{
   *pMember = *Orig.pMember;
   setTableName(Orig.tableName(), Orig.quoteTableName());
   return *this;
}

DBsqlDelete::~DBsqlDelete()
{
   delete pMember;
}

DBsqlWhere& DBsqlDelete::addWhereClause()
{
   COLPRECONDITION(pMember->HasWhereClause == false);
   pMember->HasWhereClause = true;
   return pMember->WhereClause;
}

void DBsqlDelete::removeWhereClause()
{
   COLPRECONDITION(pMember->HasWhereClause == true);
   pMember->HasWhereClause = false;
   pMember->WhereClause = DBsqlWhere();
}

DBsqlWhere& DBsqlDelete::whereClause() const
{
   COLPRECONDITION(pMember->HasWhereClause == true);
   return pMember->WhereClause;
}

const bool DBsqlDelete::whereClauseExists() const
{
   return pMember->HasWhereClause;
}
