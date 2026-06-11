#ifndef __DB_SQL_WHERE_ITEM_H__
#define __DB_SQL_WHERE_ITEM_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlWhereItem
//
// Description:
//
// A class to represent an "expression item" inside a WHERE clause. An
// "expression item" is defined as item 3) in the design manifesto below.
//
// The design of the WHERE clause hierarchy is as follows:
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

class DBsqlWhere;
class DBsqlWhereCondition;
class DBsqlWhereItemPrivate;

class DBsqlWhereItem
{
public:
   DBsqlWhereItem();
   DBsqlWhereItem( const DBsqlWhereItem& Orig );
   DBsqlWhereItem& operator=( const DBsqlWhereItem& Orig );

   virtual ~DBsqlWhereItem();
   
   // Since each "item" is meant to represent a single entity in the WHERE 
   // clause sub-expression, an "item" may be defined as only one of the 
   // following types.
   enum eWhereItemType
   {
      ITEM_TYPE_CONDITION,    // Default
      ITEM_TYPE_COMBINER,
      ITEM_TYPE_NESTED_WHERE
   };

   // The valid "Combiners" to join WHERE clause "items" together to
   // form a sub-expression.
   enum eWhereItemCombiner
   {
      AND_COMBINER,
      OR_COMBINER
   };

   // Returns the type for this "item" in a WHERE clause sub-expression.
   const eWhereItemType itemType() const;

   // Define this "item" as a DBsqlWhereCondition, which is explained in 
   // item 4) in the design manifesto above.
   DBsqlWhereCondition& setItemAsCondition();

   // Access the "condition" object for this "item" through the 
   // returned reference.
   DBsqlWhereCondition& condition() const;

   // Define this "item" as a "combiner", which is used to join 
   // WHERE clause items together to form a sub-expression.
   void setItemAsCombiner( eWhereItemCombiner Combiner );
   
   // Access the "combiner" value for this "item" through the
   // returned reference.
   const eWhereItemCombiner combiner() const;

   // Define this "item" as a "nested WHERE expression", to form a 
   // sub-expression under the current WHERE expression.
   DBsqlWhere& setItemAsNestedWhere();
   
   // Access the "nested WHERE expression" for this "item" through
   // the returned reference.
   DBsqlWhere& nestedWhere() const;

private:
   DBsqlWhereItemPrivate* pMember;
};

#endif // end of defensive include
