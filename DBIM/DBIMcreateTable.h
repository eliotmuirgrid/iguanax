#ifndef __DBIM_CREATE_TABLE_H__
#define __DBIM_CREATE_TABLE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMcreateTable
//
// Description:
//
// Database api agnostic interface for generating create table sql statements
//
// Author: Vismay Shah 
// Date:   Tuesday 07 March 2023 - 01:21PM
// ---------------------------------------------------------------------------
#include <COL/COLstring.h>
#include <COL/COLarray.h>
#include <DB/DBdatabaseType.h>
#include <DB/DBgrammar.h>

class DBIMtableColumn{
public:
   DBIMtableColumn(const COLstring& Name, DBsimpleType Type, bool Key) : m_ColumnName(Name), m_ColumnType(Type), m_IsKey(Key) {}
   COLstring    m_ColumnName;
   DBsimpleType m_ColumnType;
   bool         m_IsKey;
};

class DBIMcreateTable{
public:
   DBIMcreateTable(const COLstring& Name) : m_Name(Name) {};
   COLstring m_Name;
   COLarray<DBIMtableColumn> m_Columns;
};

void DBIMcreateTableAddColumn(DBIMcreateTable* pTable, DBIMtableColumn& Column);
COLstring DBIMgenerateCreateStatements(const COLarray<DBIMcreateTable>& TablesToCreate, const COLmap<COLstring, bool>& ExistingTables, const DBdatabaseType& Type);

#endif // end of defensive include
