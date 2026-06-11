//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlSelect
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

#include "DBsqlSelectJoin.h"
#include "DBsqlSelect.h"
#include "DBsqlSelectColumn.h"
#include "DBsqlSelectNameAlias.h"
#include "DBsqlSelectOrderBy.h"
#include "DBsqlWhere.h"

#include <COL/COLarray.h>

class DBsqlSelectPrivate
{
public:
   DBsqlSelectPrivate() 
   : DistinctFlag(false),
//   QuoteGroupByColumnNameVector(false),
     HasWhereClause(false),
     HasJoinClause(false),
     HasHavingClause(false)
   {}

   ~DBsqlSelectPrivate() {}
   
   bool DistinctFlag;

   COLarray< DBsqlSelectNameAlias > TableVector; 
   COLarray< DBsqlSelectColumn > ColumnVector;
   COLarray< COLstring > GroupByColumnNameVector;
   COLarray< bool > QuoteGroupByColumnNameVector;
   COLarray< DBsqlSelectOrderBy > OrderByVector;

   bool HasWhereClause;
   DBsqlWhere WhereClause;

   bool HasJoinClause;
   DBsqlSelectJoin JoinClause;

   bool HasHavingClause;
   DBsqlWhere HavingClause;

   void synchronizeGroupByVectors();
};

void DBsqlSelectPrivate::synchronizeGroupByVectors()
{
   // For backwards compatibility with old serialized SQL statements 
   // that don't have a QuoteGroupByColumnNameVector.
   if ( QuoteGroupByColumnNameVector.size() != GroupByColumnNameVector.size() )
   {
      for ( COLuint32 ColumnIndex = 0; ColumnIndex < GroupByColumnNameVector.size(); ColumnIndex++ )
      {
         QuoteGroupByColumnNameVector.push_back( false );
      }
   }
}

DBsqlSelect::DBsqlSelect()
{
   pMember = new DBsqlSelectPrivate();
}

DBsqlSelect::DBsqlSelect( const DBsqlSelect& Orig )
{
   pMember = new DBsqlSelectPrivate(*Orig.pMember);
}

DBsqlSelect& DBsqlSelect::operator=( const DBsqlSelect& Orig )
{
   *pMember = *Orig.pMember;
   return *this;
}

DBsqlSelect::~DBsqlSelect()
{
   delete pMember;
}

void DBsqlSelect::setDistinctFlag( bool DistinctFlag )
{
   pMember->DistinctFlag = DistinctFlag;
}

const bool& DBsqlSelect::distinctFlag() const
{
   return pMember->DistinctFlag;
}

const COLuint32 DBsqlSelect::addTable
( 
   const COLstring& TableName, 
   const COLstring& TableAlias, 
   bool QuoteName, 
   bool QuoteAlias
)
{
   COLPRECONDITION( !TableName.is_null() );
   
   pMember->TableVector.push_back();
   COLuint32 AddedTableIndex = pMember->TableVector.size() - 1;
   pMember->TableVector[ AddedTableIndex ].setName( TableName );
   pMember->TableVector[ AddedTableIndex ].setQuoteName( QuoteName );
   pMember->TableVector[ AddedTableIndex ].setAlias( TableAlias );
   pMember->TableVector[ AddedTableIndex ].setQuoteAlias( QuoteAlias );
   return AddedTableIndex;
}

const COLstring& DBsqlSelect::tableName( COLuint32 TableIndex ) const
{
   COLPRECONDITION( TableIndex < pMember->TableVector.size() )
   return pMember->TableVector[ TableIndex ].name();
}

bool DBsqlSelect::quoteTableName( COLuint32 TableIndex ) const
{
   COLPRECONDITION( TableIndex < pMember->TableVector.size() )
   return pMember->TableVector[ TableIndex ].quoteName();
}

const COLstring& DBsqlSelect::tableAlias( COLuint32 TableIndex ) const
{
   COLPRECONDITION( TableIndex < pMember->TableVector.size() )
   return pMember->TableVector[ TableIndex ].alias();
} 

bool DBsqlSelect::quoteTableAlias( COLuint32 TableIndex ) const
{
   COLPRECONDITION( TableIndex < pMember->TableVector.size() )
   return pMember->TableVector[ TableIndex ].quoteAlias();
}

const COLuint32 DBsqlSelect::countOfTableName() const
{
   return pMember->TableVector.size();
}

const COLuint32 DBsqlSelect::addColumn( const COLstring& ColumnName, 
                                        const COLstring& ColumnAlias,
                                        DBdataType ColumnCastType,
                                        bool QuoteName,
                                        bool QuoteAlias )
{
   COLPRECONDITION( !ColumnName.is_null() );

   pMember->ColumnVector.push_back();
   COLuint32 AddedColumnIndex = pMember->ColumnVector.size() - 1;
   pMember->ColumnVector[ AddedColumnIndex ].setName( ColumnName );
   pMember->ColumnVector[ AddedColumnIndex ].setQuoteName( QuoteName );
   pMember->ColumnVector[ AddedColumnIndex ].setAlias( ColumnAlias );
   pMember->ColumnVector[ AddedColumnIndex ].setQuoteAlias( QuoteAlias );
   pMember->ColumnVector[ AddedColumnIndex ].setCastType( ColumnCastType );
   return AddedColumnIndex;
}

void DBsqlSelect::clearColumns()
{
   pMember->ColumnVector.clear();
}

const COLstring& DBsqlSelect::columnName( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->ColumnVector.size() )
   return pMember->ColumnVector[ ColumnIndex ].name();
}

bool DBsqlSelect::quoteColumnName( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->ColumnVector.size() )
   return pMember->ColumnVector[ ColumnIndex ].quoteName();
}

const COLstring& DBsqlSelect::columnAlias( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->ColumnVector.size() )
   return pMember->ColumnVector[ ColumnIndex ].alias();
}

bool DBsqlSelect::quoteColumnAlias( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->ColumnVector.size() )
   return pMember->ColumnVector[ ColumnIndex ].quoteAlias();
}

DBdataType DBsqlSelect::columnCastType( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->ColumnVector.size() )
   return pMember->ColumnVector[ ColumnIndex ].castType();
}
 
const COLuint32 DBsqlSelect::countOfColumn() const
{
   return pMember->ColumnVector.size();
}

DBsqlWhere& DBsqlSelect::addWhereClause()
{
   COLPRECONDITION(!whereClauseExists());
   pMember->HasWhereClause = true;
   return pMember->WhereClause;
}

DBsqlWhere& DBsqlSelect::whereClause() const
{
   COLPRECONDITION(whereClauseExists());
   return pMember->WhereClause;
}

const bool DBsqlSelect::whereClauseExists() const
{
   return pMember->HasWhereClause;
}

DBsqlSelectJoin& DBsqlSelect::addJoinClause()
{
   COLPRECONDITION(!joinClauseExists());
   pMember->HasJoinClause = true;
   return pMember->JoinClause;
}

DBsqlSelectJoin& DBsqlSelect::joinClause() const
{
   COLPRECONDITION(joinClauseExists());
   return pMember->JoinClause;
}

const bool DBsqlSelect::joinClauseExists() const
{
   return pMember->HasJoinClause;
}

const COLuint32 DBsqlSelect::addGroupByColumnName( const COLstring& ColumnName,
                                                   bool QuoteName )
{
   COLPRECONDITION( ColumnName.size() > 0 );
   
   pMember->synchronizeGroupByVectors();

   pMember->GroupByColumnNameVector.push_back(ColumnName);
   pMember->QuoteGroupByColumnNameVector.push_back( QuoteName );
   return pMember->GroupByColumnNameVector.size() - 1;
}

const COLstring& DBsqlSelect::groupByColumnName( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->GroupByColumnNameVector.size() )
   return pMember->GroupByColumnNameVector[ ColumnIndex ];
}

bool DBsqlSelect::quoteGroupByColumnName( COLuint32 ColumnIndex ) const
{
   pMember->synchronizeGroupByVectors();
   
   COLPRECONDITION( ColumnIndex < pMember->QuoteGroupByColumnNameVector.size() )
   return pMember->QuoteGroupByColumnNameVector[ ColumnIndex ];
}

const COLuint32 DBsqlSelect::countOfGroupByColumnName() const
{
   return pMember->GroupByColumnNameVector.size();
}

void DBsqlSelect::addHavingClause()
{
   COLPRECONDITION(!havingClauseExists());
   pMember->HasHavingClause = true;
}

DBsqlWhere& DBsqlSelect::havingClause() const
{
   COLPRECONDITION(havingClauseExists());
   return pMember->HavingClause;
}

const bool DBsqlSelect::havingClauseExists() const
{
   return pMember->HasHavingClause;
}

const COLuint32 DBsqlSelect::addOrderByColumn( const COLstring ColumnName, 
                                               bool DecendingFlag,
                                               bool QuoteName )
{
   COLPRECONDITION( ColumnName.size() > 0 );

   pMember->OrderByVector.push_back();
   COLuint32 AddedOrderByIndex = pMember->OrderByVector.size() - 1;
   pMember->OrderByVector[ AddedOrderByIndex ].setColumnName( ColumnName );
   pMember->OrderByVector[ AddedOrderByIndex ].setDescendingFlag( DecendingFlag );
   pMember->OrderByVector[ AddedOrderByIndex ].setQuoteName( QuoteName );
   return AddedOrderByIndex;
}

DBsqlSelectOrderBy& DBsqlSelect::orderByColumn( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->OrderByVector.size() )
   return pMember->OrderByVector[ ColumnIndex ];
}

const COLstring& DBsqlSelect::orderByColumnName( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->OrderByVector.size() )
   return pMember->OrderByVector[ ColumnIndex ].columnName();
}

bool DBsqlSelect::quoteOrderByColumnName( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->OrderByVector.size() )
   return pMember->OrderByVector[ ColumnIndex ].quoteName(); 
}

void DBsqlSelect::setOrderByDescendingFlag( COLuint32 ColumnIndex, bool DecendingFlag )
{
   COLPRECONDITION( ColumnIndex < pMember->OrderByVector.size() )
   pMember->OrderByVector[ ColumnIndex ].setDescendingFlag( DecendingFlag );   
}

bool DBsqlSelect::orderByDescendingFlag( COLuint32 ColumnIndex ) const
{
   COLPRECONDITION( ColumnIndex < pMember->OrderByVector.size() )
   return pMember->OrderByVector[ ColumnIndex ].descendingFlag();
}

const COLuint32 DBsqlSelect::countOfOrderByColumn() const
{
   return pMember->OrderByVector.size();
}

void DBsqlSelect::clearOrderByClause()
{
   pMember->OrderByVector.clear();
}

