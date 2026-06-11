//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ICOLcolumns.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/03/25 1:50 PM
//  ---------------------------------------------------------------------------
#include <ICOL/ICOLcolumns.h>
#include <ICOL/ICOLmap.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

// instance/columns
void ICOLcolumns(const COLwebRequest& Request) {
   COL_FUNCTION(ICOLcolumns);
   COLvar Result;
   ICOLmapToVar(&Result);
   COLrespondSuccess(Request.Address, Result);
}