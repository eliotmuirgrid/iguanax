#ifndef __DBIM_COLUMN_TYPE_H__
#define __DBIM_COLUMN_TYPE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMcolumnType
//
// Description:
//
// Converts the many specialized database column types into the DBsimpleType
// supported by our database table grammar model.
//
// Author: Vismay Shah 
// Date:   Thursday 23 February 2023 - 06:22PM
// ---------------------------------------------------------------------------
#include <DB/DBgrammar.h>
class DBvariant;

// Should be called once to initialize an un-exposed static hashmap used by DBIMcolumnType.
// Tolerant of being called more than once.
void DBIMinitTypeLookup(); 
// Determines the "simple type" by checking an un-exposed static hashmap for preset column types
// If the specified type is not found in the hashmap, we interpret it as DBS_STRING
DBsimpleType DBIMcolumnType(const DBvariant& Value);


#endif // end of defensive include
