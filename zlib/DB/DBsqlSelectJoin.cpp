//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlSelectJoin
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

#include "DBsqlSelectJoin.h"
#include "DBsqlSelectNameAlias.h"
#include "DBsqlWhere.h"

class DBsqlSelectJoinPrivate
{
public:
   DBsqlSelectJoinPrivate() 
   : HasOnExpression(false),
     pNestedJoin(NULL),
     pCascadedJoin(NULL),
     JoinType(DBsqlSelectJoin::LEFT_OUTER)
   {}
   
   ~DBsqlSelectJoinPrivate() {}
   
   DBsqlSelectJoin::eJoinType JoinType;
   DBsqlSelectNameAlias  LeftTable;
   DBsqlSelectNameAlias  RightTable;

   bool HasOnExpression;
   DBsqlWhere OnExpression;

   DBsqlSelectJoin* pNestedJoin;

   DBsqlSelectJoin* pCascadedJoin;
};


DBsqlSelectJoin::DBsqlSelectJoin()
{
   pMember = new DBsqlSelectJoinPrivate();
}

void DBsqlSelectCopyCorrect(DBsqlSelectJoinPrivate& Copy)
{
   if (Copy.pNestedJoin)
   {
      Copy.pNestedJoin = new DBsqlSelectJoin(*Copy.pNestedJoin);
   }
   if (Copy.pCascadedJoin)
   {
      Copy.pCascadedJoin = new DBsqlSelectJoin(*Copy.pCascadedJoin);
   }
}


DBsqlSelectJoin::DBsqlSelectJoin( const DBsqlSelectJoin& Orig )
{
   pMember = new DBsqlSelectJoinPrivate(*Orig.pMember);
   DBsqlSelectCopyCorrect(*pMember);
}

DBsqlSelectJoin& DBsqlSelectJoin::operator=( const DBsqlSelectJoin& Orig )
{
   if (&Orig != this)
   {
      *pMember = *Orig.pMember;
      DBsqlSelectCopyCorrect(*pMember);
   }
   return *this;
}

DBsqlSelectJoin::~DBsqlSelectJoin()
{
   delete pMember->pNestedJoin;
   delete pMember->pCascadedJoin;
   delete pMember;
}

void DBsqlSelectJoin::setJoinType( const DBsqlSelectJoin::eJoinType JoinType )
{
   pMember->JoinType = JoinType;
}

const DBsqlSelectJoin::eJoinType DBsqlSelectJoin::joinType() const
{
   return pMember->JoinType;
}

void DBsqlSelectJoin::setLeftTable
( 
   const COLstring& TableName,
   const COLstring& TableAlias,
   bool QuoteName,
   bool QuoteAlias
)
{
   COLPRECONDITION( !TableName.is_null() );

   pMember->LeftTable.setName( TableName );
   pMember->LeftTable.setQuoteName( QuoteName );
   pMember->LeftTable.setAlias( TableAlias );
   pMember->LeftTable.setQuoteAlias( QuoteAlias );
}

const COLstring& DBsqlSelectJoin::leftTableName() const
{
   return pMember->LeftTable.name();
}

bool DBsqlSelectJoin::quoteLeftTableName() const
{
   return pMember->LeftTable.quoteName();
}

const COLstring& DBsqlSelectJoin::leftTableAlias() const
{
   return pMember->LeftTable.alias();
}

bool DBsqlSelectJoin::quoteLeftTableAlias() const
{
   return pMember->LeftTable.quoteAlias();
}

void DBsqlSelectJoin::setRightTable
( 
   const COLstring& TableName,
   const COLstring& TableAlias,
   bool QuoteName,
   bool QuoteAlias
)
{
   COLPRECONDITION( !TableName.is_null() );
   COLPRECONDITION( !TableName.is_null() );

   pMember->RightTable.setName( TableName );
   pMember->RightTable.setQuoteName( QuoteName );
   pMember->RightTable.setAlias( TableAlias );
   pMember->RightTable.setQuoteAlias( QuoteAlias );
}

const COLstring& DBsqlSelectJoin::rightTableName() const
{
   return pMember->RightTable.name();
}

bool DBsqlSelectJoin::quoteRightTableName() const
{
   return pMember->RightTable.quoteName();
}

const COLstring& DBsqlSelectJoin::rightTableAlias() const
{
   return pMember->RightTable.alias();
}

bool DBsqlSelectJoin::quoteRightTableAlias() const
{
   return pMember->RightTable.quoteAlias();
}

DBsqlWhere& DBsqlSelectJoin::addOnExpression()
{
   COLPRECONDITION(!onExpressionExists())
   pMember->HasOnExpression = true;;
   return pMember->OnExpression;
}

DBsqlWhere& DBsqlSelectJoin::onExpression() const
{
   COLPRECONDITION(onExpressionExists());
   return pMember->OnExpression;
}

const bool DBsqlSelectJoin::onExpressionExists() const
{
   return pMember->HasOnExpression;
}

DBsqlSelectJoin& DBsqlSelectJoin::addNestedJoin()
{
   COLPRECONDITION(!nestedJoinExists());
   pMember->pNestedJoin = new DBsqlSelectJoin();
   return *pMember->pNestedJoin;
}

DBsqlSelectJoin& DBsqlSelectJoin::nestedJoin() const
{
   COLPRECONDITION(nestedJoinExists());
   return *pMember->pNestedJoin;
}

const bool DBsqlSelectJoin::nestedJoinExists() const
{
   return (pMember->pNestedJoin != NULL);
}

DBsqlSelectJoin& DBsqlSelectJoin::addCascadedJoin()
{
   COLPRECONDITION(!cascadedJoinExists());
   pMember->pCascadedJoin = new DBsqlSelectJoin();
   return *pMember->pCascadedJoin;
}

DBsqlSelectJoin& DBsqlSelectJoin::cascadedJoin() const
{
   COLPRECONDITION(cascadedJoinExists());
   return *pMember->pCascadedJoin;
}

const bool DBsqlSelectJoin::cascadedJoinExists() const
{
   return (NULL != pMember->pCascadedJoin);
}
