// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDregisterNODmap
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 16 October 2023 - 04:29PM
// ---------------------------------------------------------------------------

#include <DBD/DBDregisterNODmap.h>
#include <LUAL/LUALutils.h>
#include <NODB/NODBresultSet.h>
#include <NODB/NODBrow.h>
#include <NODB/NODBvalue.h>
#include <NODL/NODLutils.h>
#include <NOX/NOXconstant.h>
#include <NTAB/NTABcollection.h>
#include <NTAB/NTABtable.h>
#include <NTAB/NTABtableG.h>
#include <NTAB/NTABtableRow.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDregisterNODmap(){
   COL_FUNCTION(DBDregisterNODmap);
   LUALregisterLuaTypes();
   NODBregisterResultSetType();
   NODBregisterRowType();
   NODBvalueRegisterTypes();
   NODLutilsRegisterTypes();
   NOXregisterTypes();
   NTABregisterCollectionType();
   NTABregisterTableType();
   NTABtableGRegisterTypes();
   NTABregisterRowType();
}