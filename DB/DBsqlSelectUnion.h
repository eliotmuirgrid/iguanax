#ifndef __DB_SQL_SELECT_UNION_H__
#define __DB_SQL_SELECT_UNION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlSelectUnion
//
// Description: TODO: MISSING
//
//
//
// Author: Henry Tran
// Date:   January 27, 2004
//
//---------------------------------------------------------------------------

class DBsqlSelect;
class DBsqlSelectUnionPrivate;
class DBsqlSelectOrderBy;

class DBsqlSelectUnion
{
public:
   DBsqlSelectUnion();
   DBsqlSelectUnion( const DBsqlSelectUnion& Orig ); 
   DBsqlSelectUnion& operator=( const DBsqlSelectUnion& Orig ); 
   virtual ~DBsqlSelectUnion();

   void setAllFlag( bool AllFlag );
   const bool& allFlag() const;

   const COLuint32 addSelectQuery();

   DBsqlSelect& selectQuery( COLuint32 SelectIndex ) const;
   const COLuint32 countOfSelectQuery() const;
   
   DBsqlSelectOrderBy& addOrderByColumn();
   DBsqlSelectOrderBy& orderByColumn( COLuint32 OrderByIndex ) const;
   COLuint32 countOfOrderByColumn() const;

private:
   DBsqlSelectUnionPrivate* pMember;
};

#endif // end of defensive include
