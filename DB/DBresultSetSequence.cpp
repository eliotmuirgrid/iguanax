//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBresultSetSequence
//
// Description:
//
// Wrapper for a vector of DBresultSets. 
//
// Author: Nasron Cheong
// Date:   May 17, 2003 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBresultSet.h"
#include "DBresultSetSequence.h"

#include <COL/COLarray.h>
#include <COL/COLauto.h>

class DBresultSetSequencePrivate
{
public:
   DBresultSetSequencePrivate() {}
   ~DBresultSetSequencePrivate() {}

   COLarray< DBresultSetPtr > ResultVector;
};

DBresultSetSequence::DBresultSetSequence()
{
   pMember = new DBresultSetSequencePrivate();
}

DBresultSetSequence::~DBresultSetSequence()
{
   delete pMember;
}

DBresultSetPtr DBresultSetSequence::push_back(DBresultSetPtr pNewResultSet)
{
   pMember->ResultVector.push_back(pNewResultSet);
   return pNewResultSet;
}

COLuint32 DBresultSetSequence::countOfResultSet() const
{
   return pMember->ResultVector.size();   
}

DBresultSet& DBresultSetSequence::resultSet(COLuint32 ResultIndex)
{
   return *pMember->ResultVector[ResultIndex];
}

void DBresultSetSequence::clear()
{
   pMember->ResultVector.clear();
}
