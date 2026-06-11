//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBsqlWhereItem
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

#include "DBsqlWhere.h"
#include "DBsqlWhereCondition.h"
#include "DBsqlWhereItem.h"

#include <COL/COLarray.h>

class DBsqlWhereItemPrivate
{
public:
   DBsqlWhereItemPrivate() 
   : ItemType(DBsqlWhereItem::ITEM_TYPE_CONDITION),
     Combiner(DBsqlWhereItem::AND_COMBINER)
   {}
   
   ~DBsqlWhereItemPrivate() {}

   DBsqlWhereItem::eWhereItemType ItemType;

   // Using vectors for dynamic memory allocation and automatic ownership.
   COLarray< DBsqlWhereCondition > Condition;
   
   DBsqlWhereItem::eWhereItemCombiner Combiner;

   COLarray< DBsqlWhere > NestedWhere;
};

DBsqlWhereItem::DBsqlWhereItem()
{
   pMember = new DBsqlWhereItemPrivate();
}

DBsqlWhereItem::DBsqlWhereItem( const DBsqlWhereItem& Orig )
{
   pMember = new DBsqlWhereItemPrivate(*Orig.pMember);
}

DBsqlWhereItem& DBsqlWhereItem::operator=( const DBsqlWhereItem& Orig )
{
   *pMember = *Orig.pMember;
   return *this;
}

DBsqlWhereItem::~DBsqlWhereItem()
{
   delete pMember;
}

const DBsqlWhereItem::eWhereItemType DBsqlWhereItem::itemType() const
{
   return pMember->ItemType;
}

DBsqlWhereCondition& DBsqlWhereItem::setItemAsCondition()
{
   switch(pMember->ItemType)
   {
   case DBsqlWhereItem::ITEM_TYPE_COMBINER:
      pMember->Condition.clear();
      break;
   case DBsqlWhereItem::ITEM_TYPE_NESTED_WHERE:
      pMember->NestedWhere.clear();
      break;
   case DBsqlWhereItem::ITEM_TYPE_CONDITION:
      pMember->Condition.clear();
      break;
   }
   
   pMember->ItemType = DBsqlWhereItem::ITEM_TYPE_CONDITION;
   DBsqlWhereCondition& WhereCondition = pMember->Condition.push_back();
   return WhereCondition;
}

DBsqlWhereCondition& DBsqlWhereItem::condition() const
{
   if ( pMember->ItemType != DBsqlWhereItem::ITEM_TYPE_CONDITION )
   {
      COL_ERROR_STREAM( "This WHERE item is not defined as a condition.", COLerror::PreCondition );
   }

   // To handle the default case where item is a condition but no memory has been allocated for
   // a condition object yet. This memory allocation is performed only when explicitly specified, 
   // e.g. when calling this function or the setItemAsCondition() method above.
   if ( pMember->Condition.size() == 0 )
   {
      pMember->Condition.push_back();
   }

   return pMember->Condition[ 0 ];
}

void DBsqlWhereItem::setItemAsCombiner( DBsqlWhereItem::eWhereItemCombiner Combiner )
{
   switch(pMember->ItemType)
   {   
   case DBsqlWhereItem::ITEM_TYPE_CONDITION:
      pMember->Condition.clear();
      break;
   case DBsqlWhereItem::ITEM_TYPE_NESTED_WHERE:
      pMember->NestedWhere.clear();
      break;
   case DBsqlWhereItem::ITEM_TYPE_COMBINER:
      pMember->Condition.clear();
      break;
   } 
   pMember->ItemType = DBsqlWhereItem::ITEM_TYPE_COMBINER;
   pMember->Combiner = Combiner;
}

const DBsqlWhereItem::eWhereItemCombiner DBsqlWhereItem::combiner() const
{
   if ( pMember->ItemType != DBsqlWhereItem::ITEM_TYPE_COMBINER )
   {
      COL_ERROR_STREAM( "This WHERE item is not defined as a combiner.", COLerror::PreCondition );
   }
   return pMember->Combiner;
}

DBsqlWhere& DBsqlWhereItem::setItemAsNestedWhere()
{
   switch (pMember->ItemType)
   {
   case DBsqlWhereItem::ITEM_TYPE_CONDITION:
      pMember->Condition.clear();
      break;
   case DBsqlWhereItem::ITEM_TYPE_COMBINER:
      pMember->Condition.clear();
      break;
   case DBsqlWhereItem::ITEM_TYPE_NESTED_WHERE:
      pMember->NestedWhere.clear();
      break;
   }
   
   pMember->ItemType = DBsqlWhereItem::ITEM_TYPE_NESTED_WHERE;
   return pMember->NestedWhere.push_back();
}

DBsqlWhere& DBsqlWhereItem::nestedWhere() const
{
   if ( pMember->ItemType != DBsqlWhereItem::ITEM_TYPE_NESTED_WHERE )
   {
      COL_ERROR_STREAM( "This WHERE item is not defined as a nested WHERE.", COLerror::PreCondition );
   }
   return pMember->NestedWhere[ 0 ];
}
