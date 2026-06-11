//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODBquery
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, June 17th, 2010 @ 04:24:18 PM
//
//---------------------------------------------------------------------------
#include "NODBprecomp.h"
#pragma hdrstop

#include "NODBquery.h"
#include "NODBresultSet.h"

#include <NOD/NODplace.h>

#include <DB/DBresultSetPtr.h>
#include <DB/DBdatabase.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

NODplace* NODBexecuteQuery (const DBdatabase& Database, const COLstring& Query, COLarray<DBresultSetPtr> *ResultSetVector) {
   COL_FUNCTION(NODBexecuteQuery);

   DBresultSetPtr pResultSet = Database.executeSqlString(Query, 0, 0, ResultSetVector);

   // For databases that do not support multiple result sets, we insert the single one
   // that was returned manually.
   if (ResultSetVector->size() == 0) {
      ResultSetVector->push_back(pResultSet);
   }

   return (pResultSet.get() != NULL) ? new NODBresultSet(pResultSet.get()) : NULL;
}
