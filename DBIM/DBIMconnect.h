#ifndef __DBIM_CONNECT_H__
#define __DBIM_CONNECT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMconnect
//
// Description:
//
// Helper functions to connect and disconnect from a database.
// 
// Author: Vismay Shah 
// Date:   Monday 16 January 2023 - 04:12PM
// ---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <DB/DBdatabasePtr.h>
class COLstring;

bool DBIMconnect(const COLstring& Datasource, const COLstring& Username, const COLstring& Password, const COLstring& Location,
                DBdatabasePtr pDatabase, COLstring* pErr);
void DBIMdisconnect(DBdatabasePtr pDatabase);

#endif // end of defensive include
