#ifndef __NODB_QUERY_H__
#define __NODB_QUERY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODBquery
//
// Description:
//
// Contains NODBexecuteQuery, a function which will execute an SQL query
// on the specified data source.
//
// Author: Kevin Senn
// Date:   Thursday, June 17th, 2010 @ 04:24:18 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLarray.h>
#include <DB/DBresultSetPtr.h>

class DBdatabase;
class NODplace;

NODplace* NODBexecuteQuery (const DBdatabase& Database, const COLstring& Query, COLarray<DBresultSetPtr> *ResultSetVector);

#endif // end of defensive include
