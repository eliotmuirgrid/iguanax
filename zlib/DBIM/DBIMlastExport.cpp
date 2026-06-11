// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBIMlastExport
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 09 November 2023 - 10:25AM
// ---------------------------------------------------------------------------

#include <DBIM/DBIMlastExport.h>
#include <DBIM/DBIMutils.h>

#include <DB/DBgrammar.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <SCK/SCKloop.h>

#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void DBIMreadLastExportImpl(COLstring Filepath, COLstring Component, COLaddress Address) {
   COL_FUNCTION(DBIMreadLastExportImpl);
   DBcollection Grammar;
   COLstring ErrorMessage, ResolvedVdb, Datasource;
   COLvar Result;
   COL_TRC("Trying to read vdb into DBcollection");
   if (!DBIMreadVdbToDbCollection(Filepath, Component, &ResolvedVdb, &Grammar, &ErrorMessage)){ return COLrespondError(Address, ErrorMessage); }
   COL_TRC("Check to see if datasource is set, if so set it to the result");
   bool ExportExists = (Grammar.Export.size() && Grammar.Export.count("datasource"));
   if (ExportExists) { 
      Result["datasource"] = Grammar.Export["datasource"]; 
   }
   COLrespondSuccess(Address, Result);
}

// /database/read_last_export
void DBIMreadLastExport(const COLwebRequest& Request, SCKloop* pLoop){
   COL_FUNCTION(DBIMreadLastExport);
   const COLvar& Input = Request.Data;
   COL_VAR(Input.json(1));
	const COLstring& Filepath  = COLvarRequiredString(Input, "filepath");
	const COLstring& Component = COLvarRequiredString(Input, "component");
   COL_TRC("Schedule read on worker thread");
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&DBIMreadLastExportImpl, Filepath, Component, Request.Address));
}