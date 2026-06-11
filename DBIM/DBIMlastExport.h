#ifndef __DBIM_LAST_EXPORT_H__
#define __DBIM_LAST_EXPORT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMlastExport
//
// Description:
//
// Api to read the "export" section from a vdb file.
//
// Author: Vismay Shah 
// Date:   Thursday 09 November 2023 - 10:25AM
// ---------------------------------------------------------------------------
class COLwebRequest;
class SCKloop;

void DBIMreadLastExport(const COLwebRequest& Request, SCKloop* pLoop);

#endif // end of defensive include
