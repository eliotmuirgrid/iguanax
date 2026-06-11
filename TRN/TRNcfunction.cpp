//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNcfunction
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, November 23rd, 2010 @ 09:33:25 AM
//---------------------------------------------------------------------------
#include "TRNcfunction.h"

#include <REX/REXutil.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


TRNcfunction::TRNcfunction(const COLstring& Name) : m_Name(Name), m_Calls(0), m_FunctionId(UNKNOWN) {
   COL_METHOD(TRNcfunction::TRNcfunction);
   COL_VAR(Name);
}

TRNcfunction::~TRNcfunction() {
   COL_METHOD(TRNcfunction::~TRNcfunction);
}

COLstring TRNcFunctionDisplayName(TRNcfunction* pFunction){
   COL_FUNCTION(TRNcFunctionDisplayName);
   return REXreplace(pFunction->m_Name, "^(.+\\.)+", "");
}

int TRNcFunctionIncrementCalls(TRNcfunction* pFunction){
   COL_FUNCTION(TRNcFunctionIncrementCalls);
   pFunction->m_Calls++;
   return pFunction->m_Calls;
}