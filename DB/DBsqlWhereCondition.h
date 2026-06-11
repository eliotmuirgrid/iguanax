#ifndef __DB_SQL_WHERE_CONDITION_H__
#define __DB_SQL_WHERE_CONDITION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlWhereCondition
//
// Description:
//
// A class to represent a "condition" in a WHERE clause expression. A
// "condition" is defined as item 4) in the design manifesto below.
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
// Date:   December 1, 2003 
//
//---------------------------------------------------------------------------

class DBvariant;
class DBsqlSelect;
class DBsqlWhereConditionPrivate;

class DBsqlWhereCondition 
{
public:
   DBsqlWhereCondition();
   DBsqlWhereCondition( const DBsqlWhereCondition& Orig ); 
   DBsqlWhereCondition& operator=( const DBsqlWhereCondition& Orig ); 
   virtual ~DBsqlWhereCondition(); // TODO - may not need to be virtual - Eliot
   
   // These are the supported "operators" to compare the "operands" of 
   // a WHERE "condition".
   enum eOperator
   {
      OPERATOR_NOT_DEFINED, // Denotes that an operator has not been set for this condition.
      EQUAL,
      NOT_EQUAL,
      LESS_THAN,
      LESS_THAN_OR_EQUAL,
      GREATER_THAN,
      GREATER_THAN_OR_EQUAL,
      LIKE, // For wildcard comparisons.
      IS_NULL,
      BETWEEN, // Specify two values for the right operand when using this comparison.
      IN_SET, // To compare with a specified set of values
      IN_SELECT // To compare with a SELECT result
   };

   // Set the negate flag for this WHERE condition. If set to
   // true, the boolean result of this condition will be negated. By
   // default this flag is set to false.
   void setNegateFlag( bool NegateFlag );

   // Retrieve the value of the negate flag for this WHERE condition.
   const bool& negateFlag() const;

   void setLeftOperandColumnName( const COLstring& ColumnName );

   const COLstring& leftOperandColumnName() const;

   void setQuoteLeftOperandColumnName( bool QuoteFlag );
   bool quoteLeftOperandColumnName() const;

   // Set the "operator" for this WHERE condition to specify the type
   // of comparison between the "operands".
   void setConditionOperator( const eOperator& Operator );

   // Retrieve the "operator" for this WHERE condition.
   const eOperator conditionOperator() const;

   void setRightOperandIsColumnFlag( bool IsColumnFlag );

   const bool& rightOperandIsColumnFlag() const;

   void setQuoteRightOperandColumnName( bool QuoteFlag );
   bool quoteRightOperandColumnName() const;

   const COLuint32 addRightOperandValue( const DBvariant& Value );

   const DBvariant& rightOperandValue( COLuint32 ValueIndex = 0 ) const;
   
   const COLuint32 countOfRightOperandValue() const;

   void addRightOperandSelectQuery();

   // TODO: - check with Henry that this is not supposed to return a const reference
   //       - naming convention: select -> Select ?
   DBsqlSelect& rightOperandselectQuery() const; 

   // TODO: naming convention: select -> Select ?
   const bool rightOperandselectQueryExists() const;

private:
   DBsqlWhereConditionPrivate* pMember;
};

#endif // end of defensive include
