//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODBresultSet
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, June 17th, 2010 @ 03:17:43 PM
//---------------------------------------------------------------------------
#include "NODBprecomp.h"
#pragma hdrstop

#include "NODBresultSet.h"
#include "NODBrow.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

NODBresultSet::NODBresultSet(DBresultSet* pResultSet) : m_pResultSet(pResultSet){
   setSize(m_pResultSet->countOfRow());
}

NODplace* NODBresultSet::newChild(int i, const char* Arg1, const char* Arg2){
   COLPRECONDITION(i == -1);
   return new NODBrow(m_pResultSet.get(), i);
}

NODplace* NODBresultSet::initChild(int i) const{
   COLPRECONDITION(i >= 0 && i < m_pResultSet->countOfRow());
   return new NODBrow(m_pResultSet.get(), i);
}

static const COLstring NODBresultSetName("Result Set");
static const COLstring NODBresultSetTypeName("result_set");

void NODBregisterResultSetType(){
   COL_FUNCTION(NODBregisterResultSetType);
   NODtypeMapAdd(NODBresultSetTypeName, NODB_RESULT_SET, NODBprotocolName, NODB_DB);
}

const COLstring& NODBresultSet::nodeTypeName() const { return NODBresultSetTypeName; }
const COLstring& NODBresultSet::name() const { return NODBresultSetName; }
