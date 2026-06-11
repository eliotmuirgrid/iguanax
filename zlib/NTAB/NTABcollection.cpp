//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABcollection
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:15:41 PM
//
//---------------------------------------------------------------------------
#include "NTABprecomp.h"
#pragma hdrstop

#include "NTABcollection.h"
#include "NTABcollectionG.h"
#include "NTABtable.h"
#include "NTABschema.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

NTABcollection::NTABcollection(const NTABcollectionG* pGrammar) :m_pGrammar(pGrammar) {
   COL_METHOD(NTABcollection);
   COLASSERT(pGrammar);
   setSize(pGrammar->countOfTable());
}

NTABcollection::~NTABcollection(){
   COL_METHOD(~NTABcollection);
}

static const COLstring NTABlblCollection("table_collection");

void NTABregisterCollectionType(){
   COL_FUNCTION(NTABregisterCollectionType);
   NODtypeMapAdd(NTABlblCollection, NTAB_ROOT, NTABprotocolName, NTAB_TABLE_PROTOCOL);
}

const COLstring& NTABcollection::nodeTypeName() const { return NTABlblCollection; }
const COLstring& NTABcollection::name() const { return m_pGrammar->name(); }

COLstring NTABcollection::treeValue() const{
   return "[collection]";
}

NODplace* NTABcollection::initChild(int i) const{
   COLASSERT(i >=0 && i < m_pGrammar->countOfTable());
   return new NTABtable(&m_pGrammar->table(i));
}

const NTABcollectionG* NTABcollectionByName(const NTABschema& Schema, const COLstring& Name){
   COL_FUNCTION(NTABcollectionByName);
   COL_VAR(Name);
   const NTABcollectionG* pCollection = Schema.tableCollection(Name);

   if (pCollection) { return pCollection; }

   if (Schema.countOfCollection() == 0){
      COL_ERROR_STREAM_PLAIN("Message name \'" << Name << COL_T("\' does not exist.  There are no messages defined in this schema file."), COLerror::PreCondition);
   }
   COLstring Error;
   COLostream Stream(Error);
   Stream << "Message name \'" << Name << COL_T("' does not exist.") << newline << (Schema.countOfCollection() > 1 ? "Messages that are defined: " : "This only message defined in the schema file is: ");  // TODO - error could be refined further.
   for (COLindex i=Schema.firstTableCollection(); i != 0; i = Schema.nextTableCollection(i)){
      Stream << '\'' << Schema.tableCollection(i).name() << "\' ";
   }
   COL_ERROR_STREAM_PLAIN(Error, COLerror::PreCondition);
   return NULL;
}
