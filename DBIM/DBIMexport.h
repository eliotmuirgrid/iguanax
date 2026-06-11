#ifndef __DBIM_EXPORT_H__
#define __DBIM_EXPORT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMexport
//
// Description:
//
// TODO
//
// Author: Vismay Shah 
// Date:   Tuesday 04 April 2023 - 10:39AM
// ---------------------------------------------------------------------------
class COLwebRequest;
class DBdatabaseFactory;
class SCKloop;

void DBIMexport(const COLwebRequest& Request, SCKloop* pLoop, DBdatabaseFactory* pDatabaseFactory);

#endif // end of defensive include
