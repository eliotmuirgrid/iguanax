//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlCommandSequence
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   May 17, 2004
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBsqlCommandSequence.h"
#include "DBsqlCommand.h"

#include <COL/COLarray.h>
#include <COL/COLauto.h>

class DBsqlCommandSequencePrivate
{
public:
   DBsqlCommandSequencePrivate() {}
   virtual ~DBsqlCommandSequencePrivate() {}
  
   COLarray< COLauto<DBsqlCommand> > CommandVector;
};

DBsqlCommandSequence::DBsqlCommandSequence()
{
   pMember = new DBsqlCommandSequencePrivate();
}

DBsqlCommandSequence::~DBsqlCommandSequence()
{
   delete pMember;
}

DBsqlCommand* DBsqlCommandSequence::push_back(DBsqlCommand* pNewCommand)
{
   return pMember->CommandVector.push_back(pNewCommand).get();   
}

COLuint32 DBsqlCommandSequence::countOfSqlCommand() const
{
   return pMember->CommandVector.size();   
}

const DBsqlCommand& DBsqlCommandSequence::sqlCommand(COLuint32 CommandIndex) const
{
   return pMember->CommandVector[CommandIndex];
}

DBsqlCommand& DBsqlCommandSequence::sqlCommand(COLuint32 CommandIndex)
{
   return pMember->CommandVector[CommandIndex];
}

void DBsqlCommandSequence::clear()
{
   pMember->CommandVector.clear();
}

/*
void DBsqlCommandSequence::append(const DBsqlCommandSequence& Next)
{
   for (COLindex CommandIndex = 0; CommandIndex < Next.countOfSqlCommand(); CommandIndex++)
   {
      pMember->CommandVector.push_back(Next.sqlCommand(CommandIndex));
   }
}
*/
