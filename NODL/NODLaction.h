#ifndef __NODL_ACTION_H__
#define __NODL_ACTION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLaction
//
// Description:
//
// Functions for performing database actions
//
// Author: Vismay Shah 
// Date:   Monday 8 December 2022
// ---------------------------------------------------------------------------
#include <COL/COLarray.h>
struct lua_State;

class COLstring;
class DBdatabasePtr;
class DBsqlCommand;
class DBsqlInsert;
class DBsqlSelect;
class DBsqlUpdate;
class NTABtable;

void NODLinsertTableRowExecute(DBdatabasePtr pDatabase, const COLarray<COLstring>& KeyFields, const DBsqlSelect& SqlSelectExistingRecords, DBsqlUpdate& SqlInsertOrUpdate, COLuint64& TotalSqlSize);
void NODLinsertTableRowPrepare(const NTABtable& Table, int RowIndex, DBsqlSelect* pSqlSelectExistingRecords, DBsqlUpdate* pSqlInsertOrUpdate, COLarray<COLstring>* pKeyFields);
void NODLdbCreateBulkStreamStatement(DBdatabasePtr pDBdatabase, COLostream& Stream, const NTABtable& Table, bool SimpleCompound);
void NODLdbCreateBulkInsertStatement(DBsqlInsert& SqlInsertCommand, const NTABtable&Table, bool SimpleCompound);
COLstring NODLactionMakeSql(const DBdatabasePtr pDatabase, const DBsqlCommand& Cmd);
#endif // end of defensive include
