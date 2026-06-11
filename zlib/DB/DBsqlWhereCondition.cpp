//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlWhereCondition
//
// Description:
//
// Implementation
// 
// Author: Henry Tran
// Date:   December 1, 2003 
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include <COL/COLarray.h>

#include "DBsqlSelect.h"
#include "DBsqlWhereCondition.h"
#include "DBvariant.h"

class DBsqlWhereConditionPrivate
{
public:
   DBsqlWhereConditionPrivate()
   : NegateFlag(false),
     RightOperandIsColumnFlag(false),
     QuoteLeftOperandColumnName(false),
     QuoteRightOperandColumnName(false),
     Operator(DBsqlWhereCondition::OPERATOR_NOT_DEFINED),
     HasSelectQuery(false)
     
   {}
   ~DBsqlWhereConditionPrivate() {}

   
   bool NegateFlag;
   COLstring LeftOperandColumnName;
   bool QuoteLeftOperandColumnName;
   DBsqlWhereCondition::eOperator Operator;
   bool RightOperandIsColumnFlag;
   bool QuoteRightOperandColumnName;
   COLarray< DBvariant > RightOperandValueVector;

   bool HasSelectQuery;
   DBsqlSelect SelectQuery;
};

DBsqlWhereCondition::DBsqlWhereCondition()
{
   pMember = new DBsqlWhereConditionPrivate();
}

DBsqlWhereCondition::DBsqlWhereCondition( const DBsqlWhereCondition& Orig )
{
   pMember = new DBsqlWhereConditionPrivate(*Orig.pMember);
}

DBsqlWhereCondition& DBsqlWhereCondition::operator=( const DBsqlWhereCondition& Orig )
{
   *pMember = *Orig.pMember;
   return *this;
}

DBsqlWhereCondition::~DBsqlWhereCondition()
{
   delete pMember;
}

void DBsqlWhereCondition::setNegateFlag( bool NegateFlag )
{
   pMember->NegateFlag = NegateFlag;
}

const bool& DBsqlWhereCondition::negateFlag() const
{
   return pMember->NegateFlag;
}

void DBsqlWhereCondition::setLeftOperandColumnName( const COLstring& ColumnName )
{
   COLPRECONDITION( !ColumnName.is_null() );
   pMember->LeftOperandColumnName = ColumnName;
}

const COLstring& DBsqlWhereCondition::leftOperandColumnName() const
{
   return pMember->LeftOperandColumnName;
}

void DBsqlWhereCondition::setConditionOperator( const eOperator& Operator )
{
   pMember->Operator = Operator;
}

const DBsqlWhereCondition::eOperator DBsqlWhereCondition::conditionOperator() const
{
   return pMember->Operator;
}

void DBsqlWhereCondition::setRightOperandIsColumnFlag( bool IsColumnFlag )
{
   pMember->RightOperandIsColumnFlag = IsColumnFlag;
}

const bool& DBsqlWhereCondition::rightOperandIsColumnFlag() const
{
   return pMember->RightOperandIsColumnFlag;
}

void DBsqlWhereCondition::setQuoteLeftOperandColumnName( bool QuoteFlag )
{
   pMember->QuoteLeftOperandColumnName = QuoteFlag;
}

bool DBsqlWhereCondition::quoteLeftOperandColumnName() const
{
   return pMember->QuoteLeftOperandColumnName;
}

const COLuint32 DBsqlWhereCondition::addRightOperandValue( const DBvariant& Value )
{
   pMember->RightOperandValueVector.push_back( Value );
   return pMember->RightOperandValueVector.size() - 1;
}

const DBvariant& DBsqlWhereCondition::rightOperandValue( COLuint32 ValueIndex ) const
{
   COLPRECONDITION( ValueIndex < pMember->RightOperandValueVector.size() )
   return pMember->RightOperandValueVector[ ValueIndex ];
}

const COLuint32 DBsqlWhereCondition::countOfRightOperandValue() const
{
   return pMember->RightOperandValueVector.size();
}

void DBsqlWhereCondition::addRightOperandSelectQuery()
{
   COLPRECONDITION(!rightOperandselectQueryExists());
   pMember->HasSelectQuery = true;
}
   
DBsqlSelect& DBsqlWhereCondition::rightOperandselectQuery() const
{
   COLPRECONDITION(rightOperandselectQueryExists());
   return pMember->SelectQuery;
}

const bool DBsqlWhereCondition::rightOperandselectQueryExists() const
{
   return pMember->HasSelectQuery;
}

void DBsqlWhereCondition::setQuoteRightOperandColumnName( bool QuoteFlag )
{
   pMember->QuoteRightOperandColumnName = QuoteFlag;
}

bool DBsqlWhereCondition::quoteRightOperandColumnName() const
{
   return pMember->QuoteRightOperandColumnName;
}
