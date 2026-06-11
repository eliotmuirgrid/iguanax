#ifndef __DB_SQL_WHERE_H__
#define __DB_SQL_WHERE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlWhere
//
// Description:
//
// A class to represent a WHERE clause, which is frequently included in 
// SQL commands such as SELECT, UPDATE, and DELETE. A WHERE clause is used to 
// filter rows for the command.
//
// The design of the WHERE clause as an object hierarchy is as follows:
//
// 1) The DBsqlWhere object contains a vector of DBsqlWhereItems. Brackets
//    will enclose thse "items" to form a sub-expression for the WHERE 
//    clause. Additionally, a negate flag may be set to negate the boolean
//    value of this sub-expression.
//
// 2) Each DBsqlWhereItem may in turn be set as either a DBsqlWhereCondition,
//    a "combiner", or a nested DBsqlWhere sub-expression.
//
// 3) The description for each type of DBsqlWhereItem follows:
// 3a) A DBsqlWhereCondition consists of left and right DBsqlWhereOperands and 
//     an "operator". Each comparison in the WHERE clause is represented
//     as a DBsqlWhereCondition. Note that some comparisons such as "IS_NULL" 
//     only requires a left DBsqlWhereOperand.
// 3b) A "combiner" can be either "AND" or "OR", and is used to join 
//     DBsqlWhereItems together.
// 3c) A nested DBsqlWhere sub-expression is the same type of object as in 1),
//     so it also contains a vector of DBsqlWhereItems.
//
// 4) A description of the DBsqlWhereCondition parameters follows:
// 4a) A DBsqlWhereOperand normally consists of the operand's data type and the
//     operand's value. More than one value may be specified when comparing with
//     a set of values.
//     Another possibility is to use a SELECT query as an operand, for comparing
//     with values from a SELECT query result ("IN_SELECT" operator).
// 4b) An "operator" is defined as one of DBsqlWhereCondition::eOperator, and 
//     is used to specify the particular comparison between the left and right 
//     "operands".
//
// Author: Henry Tran
// Date:   November 27, 2003 
//
//---------------------------------------------------------------------------

class DBsqlWhereItem;
class DBsqlWherePrivate;

class DBsqlWhere 
{
public:
   DBsqlWhere();
   DBsqlWhere( const DBsqlWhere& Orig ); 
   DBsqlWhere& operator=( const DBsqlWhere& Orig ); 
   virtual ~DBsqlWhere();

   // Set the negate flag for this WHERE clause sub-expression. If set to
   // true, the boolean result of this sub-expression will be negated. By
   // default this flag is set to false.
   void setNegateFlag( bool NegateFlag );

   // Retrieve the value of the negate flag for this WHERE 
   // clause sub-expression.
   bool negateFlag() const;   

   // Add a DBsqlWhereItem to this WHERE clause, as explained in item 2) in
   // the design manifesto above.
   DBsqlWhereItem& addItem();
   
   // Access each DBsqlWhereItem of the WHERE clause sub-expression 
   // by vector index.
   DBsqlWhereItem& item( COLuint32 ItemIndex ) const;

   // The current size of the DBsqlWhereItem vector for this WHERE clause
   // sub-expression.
   COLuint32 countOfItem() const;

private:
   DBsqlWherePrivate* pMember;
};

#endif // end of defensive include
