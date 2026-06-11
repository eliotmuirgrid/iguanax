#ifndef __DB_SQL_SELECT_JOIN_H__
#define __DB_SQL_SELECT_JOIN_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlSelectJoin
//
// Description:
//
// A class to encapsulate the JOIN clause of a SELECT query into a 
// single object, which may consist of table names, nested JOINs, and/or
// cascaded JOINs.
//
// An example of a nested JOIN: 
// SELECT * FROM tbl1 RIGHT JOIN (tbl2 LEFT JOIN tbl3 ON tab2.c1 = tab3.c1) ON tab1.c3 = tab2.c3
//
// where "(tbl2 LEFT JOIN tbl3 ON tab2.c1 = tab3.c1)" is a nested JOIN.
//
// An example of a cascaded JOIN:
// SELECT * FROM (tbl1 LEFT JOIN tbl2 ON tbl1.c3 = tbl2.c3) RIGHT JOIN tab3 ON tab1.c3 = tab3.c3
//
// where "RIGHT JOIN tab3 ON tab1.c3 = tab3.c3" is a cascaded JOIN.
//
// Author: Henry Tran
// Date:   January 27, 2004
//
//---------------------------------------------------------------------------

class DBsqlWhere;
class DBsqlSelectJoinPrivate;

class DBsqlSelectJoin
{
public:
   DBsqlSelectJoin();
   DBsqlSelectJoin( const DBsqlSelectJoin& Orig ); 
   DBsqlSelectJoin& operator=( const DBsqlSelectJoin& Orig ); 
   virtual ~DBsqlSelectJoin();

   // These are the supported types of JOINs.
   enum eJoinType
   {
      LEFT_OUTER,
      RIGHT_OUTER,
      FULL_OUTER,
      INNER,
      CROSS
   };

   // Set the type for this JOIN clause as one of the types defined in
   // DBsqlSelectJoin::eJoinType.
   void setJoinType( const eJoinType JoinType );
   
   // Retrieve the type for this JOIN clause.
   const eJoinType joinType() const;

   // Specify a table name that will be placed on the left-hand side of
   // the JOIN clause.
   void setLeftTable( const COLstring& TableName,
                      const COLstring& TableAlias = "",
                      bool QuoteName = false,
                      bool QuoteAlias = false );
   
   // Retrieve the table name that will be placed on the left-hand side of
   // the JOIN clause.
   const COLstring& leftTableName() const;
   bool quoteLeftTableName() const;

   const COLstring& leftTableAlias() const;
   bool quoteLeftTableAlias() const;

   // Specify a table name that will be placed on the right-hand side of
   // the JOIN clause.
   void setRightTable( const COLstring& TableName,
                       const COLstring& TableAlias = "",
                       bool QuoteName = false,
                       bool QuoteAlias = false);
   
   // Retrieve the table name that will be placed on the right-hand side of
   // the JOIN clause.
   const COLstring& rightTableName() const;
   bool quoteRightTableName() const;

   const COLstring& rightTableAlias() const;
   bool quoteRightTableAlias() const;
   
   // Add an ON expression to specify the condition(s) for this JOIN 
   // clause. Returns the ON expression object that was added.
   DBsqlWhere& addOnExpression();
   
   // Access the ON expression object for this JOIN clause by reference.
   DBsqlWhere& onExpression() const;
   
   // Returns true if an ON expression has been added to this JOIN clause.
   const bool onExpressionExists() const;

   // Add a nested JOIN to this JOIN clause. An example of a nested 
   // JOIN is given in the comment block above. Returns the nested JOIN 
   // object that was added.
   DBsqlSelectJoin& addNestedJoin();
   
   // Access the nested JOIN object for this JOIN clause by reference.
   DBsqlSelectJoin& nestedJoin() const;

   // Returns true if a nested JOIN has been added to this JOIN clause.
   const bool nestedJoinExists() const;

   
   // Add a cascaded JOIN to this JOIN clause. An example of a cascaded 
   // JOIN is given in the comment block above. Returns the cascaded JOIN 
   // object that was added.
   DBsqlSelectJoin& addCascadedJoin();
   
   // Access the cascaded JOIN object for this JOIN clause by reference.
   DBsqlSelectJoin& cascadedJoin() const;
   
   // Returns true if a cascaded JOIN has been added to this JOIN clause.
   const bool cascadedJoinExists() const;

private:
   DBsqlSelectJoinPrivate* pMember;
};

#endif // end of defensive include
