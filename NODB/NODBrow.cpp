//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODBrow
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, June 17th, 2010 @ 03:18:13 PM
//
//---------------------------------------------------------------------------
#include "NODBprecomp.h"
#pragma hdrstop

#include "NODBrow.h"
#include "NODBvalue.h"

#include <DB/DBvariant.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

NODBrow::NODBrow(DBresultSet* pResultSet, int Row) : m_pResultSet(pResultSet), m_Row(Row){
   setSize(m_pResultSet->countOfColumn());
}

NODplace* NODBrow::initChild(int i) const{
   COLPRECONDITION(i == -1 || (i >= 0 && i < m_pResultSet->countOfColumn()));
   return new NODBvalue(m_pResultSet.get(), m_Row, i);
}

int NODBrow::childIndex(const COLstring& Name) const{
   return m_pResultSet->columnIndex(Name);
}

static COLstring NODBrowName("Row");
static COLstring NODBrowTypeName("row");

void NODBregisterRowType(){
   COL_FUNCTION(NODBregisterRowType);
   NODtypeMapAdd(NODBrowTypeName, NODB_ROW, NODBprotocolName, NODB_DB);
}

const COLstring& NODBrow::nodeTypeName() const { return NODBrowTypeName; }
const COLstring& NODBrow::name() const { return NODBrowName; }
