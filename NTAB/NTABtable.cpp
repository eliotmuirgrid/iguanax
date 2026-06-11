//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABtable
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:15:48 PM
//
//---------------------------------------------------------------------------
#include "NTABprecomp.h"
#pragma hdrstop

#include "NTABtable.h"
#include "NTABtableG.h"
#include "NTABtableRow.h"

#include <NOD/NODoperation.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

NTABtable::NTABtable(const NTABtableG* pGrammar): m_pGrammar(pGrammar){
   COL_METHOD(NTABtable);
}

NTABtable::~NTABtable(){
   COL_METHOD(~NTABtable);
}

static const COLstring NTABtableName("table");

void NTABregisterTableType(){
   COL_FUNCTION(NTABregisterTableType);
   NODtypeMapAdd(NTABtableName, NTAB_TABLE, NTABprotocolName, NTAB_TABLE_PROTOCOL);
}

const COLstring& NTABtable::nodeTypeName() const { return NTABtableName;}

const COLstring& NTABtable::name() const { return m_pGrammar->name(); }

NODplace* NTABtable::newChild(int i, const char* Arg1, const char* Arg2){
   if (Arg1 != NULL || Arg2 != NULL){
      COL_ERROR_STREAM_PLAIN("", NOD_BAD_ARGUMENTS);
   }
   return new NTABtableRow(m_pGrammar);
}

NODplace* NTABtable::initChild(int i) const{
   COL_METHOD(NTABtable::initChild);
   // This method is only for nodes that start with data in them.
   COLASSERT(false);
   return NULL;
}
