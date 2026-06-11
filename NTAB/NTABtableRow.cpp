//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABtableRow
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:15:51 PM
//
//---------------------------------------------------------------------------
#include "NTABprecomp.h"
#pragma hdrstop

#include "NTABtableRow.h"
#include "NTABtableItem.h"
#include "NTABtableG.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

NTABtableRow::NTABtableRow(const NTABtableG* pGrammar) : m_pGrammar((NTABtableG*)pGrammar), m_IsNull(true) {
   setSize(pGrammar->countOfColumn());
}

static const COLstring NTABrowString("row");

void NTABregisterRowType(){
   COL_FUNCTION(NTABregisterRowType);
   NODtypeMapAdd(NTABrowString, NTAB_TABLE_ROW, NTABprotocolName, NTAB_TABLE_PROTOCOL);
}

const COLstring& NTABtableRow::nodeTypeName() const{ return NTABrowString; }

const COLstring& NTABtableRow::name() const{
   return m_pGrammar->rowName();
}

NODplace* NTABtableRow::initChild(int Column) const{
   return NTABnewTableItem(m_pGrammar.get(), Column);
}

int NTABtableRow::childIndex(const COLstring& Name) const{
   return m_pGrammar->columnIndex(Name);
}


bool NTABtableRow::onRemoveChild(int i){
   COL_FUNCTION(onRemoveChild);
   if (m_pGrammar->countOfRef() > 1){
      COL_TRC("Cloning the table grammar.");
      m_pGrammar = new NTABtableG(*m_pGrammar);  // clone    
   } else { COL_TRC("No need to clone grammar."); }
   m_pGrammar->removeColumn(i);
   return true;
}
