//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlUpdate
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

#include "DBsqlUpdate.h"
#include "DBsqlWhere.h"

class DBsqlUpdatePrivate
{
public:
   DBsqlUpdatePrivate() 
   : HasWhereClause(false)
   {}

   ~DBsqlUpdatePrivate() {}
   
   bool HasWhereClause;
   DBsqlWhere WhereClause;
};

DBsqlUpdate::DBsqlUpdate()
: CommandType(DB_UPDATE_CMD)
{
   pMember = new DBsqlUpdatePrivate();
}

DBsqlUpdate::DBsqlUpdate( const DBsqlUpdate& Orig )
: DBsqlInsert(Orig),
  CommandType(Orig.CommandType)
{
   pMember = new DBsqlUpdatePrivate(*Orig.pMember);
}

DBsqlUpdate& DBsqlUpdate::operator=( const DBsqlUpdate& Orig )
{
   DBsqlInsert::operator=(Orig);
   *pMember = *Orig.pMember;
   CommandType = Orig.CommandType;
   return *this;
}

DBsqlUpdate::~DBsqlUpdate()
{
   delete pMember;
}

// Switch type of operation to INSERT
void DBsqlUpdate::setMakeIntoInsert()
{
   CommandType = DB_INSERT_CMD;
}

void DBsqlUpdate::setMakeIntoUpdate()
{
   CommandType = DB_UPDATE_CMD;
}

DBsqlWhere& DBsqlUpdate::addWhereClause()
{
   COLPRECONDITION(!whereClauseExists());
   pMember->HasWhereClause = true;
   return pMember->WhereClause;
}

DBsqlWhere& DBsqlUpdate::whereClause() const
{
   COLPRECONDITION(whereClauseExists());
   return pMember->WhereClause;
}

const bool DBsqlUpdate::whereClauseExists() const
{
   return pMember->HasWhereClause;
}
