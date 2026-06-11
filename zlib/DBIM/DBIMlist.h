#ifndef __DBIM_LIST_H__
#define __DBIM_LIST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMlist
//
// Description:
//
// Lists the databases currently supported in the database factory. This is 
// based on which drivers have been set up in Iguana.
//
// Author: Vismay Shah 
// Date:   Wednesday 08 March 2023 - 09:00AM
// ---------------------------------------------------------------------------
class COLwebRequest;
class DBdatabaseFactory;

void DBIMlist(const COLwebRequest& Request, DBdatabaseFactory* pDatabaseFactory);

#endif // end of defensive include
