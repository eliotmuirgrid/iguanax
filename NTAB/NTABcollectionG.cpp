//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABcollectionG
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:18:14 PM
//
//---------------------------------------------------------------------------
#include "NTABprecomp.h"
#pragma hdrstop

#include "NTABcollectionG.h"
#include "NTABschema.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;


NTABcollectionG::NTABcollectionG(const NTABschema& Schema, const COLstring& Name)
: m_Schema(Schema), m_Name(Name) { 
   COL_METHOD(NTABcollectionG);
}

NTABcollectionG::~NTABcollectionG(){
   COL_METHOD(~NTABcollectionG);
}
   
const NTABtableG& NTABcollectionG::table(int i) const{
   return m_Schema.table(m_TableSet[i]);
}

int NTABcollectionG::tableIndex(int i) const { return m_TableSet[i]; }

void NTABcollectionG::setTableIndex(int i, int j){
   m_TableSet[i] = j;
}

void NTABcollectionG::addTable(const COLstring& Name){
   COL_METHOD(NTABcollectionG::addTable);
   int i = m_Schema.tableIndex(Name);
   for (int j=0; j< countOfTable(); j++){
      if (m_TableSet[j] == i){
         COL_TRC("We have table " << Name << " already.");
         return;
      }
   }
   m_TableSet.push_back(i);
}



COLostream& operator<<(COLostream& Stream, const NTABcollectionG& Collection){
   COL_FUNCTION(NTABcollectionG<<);
   int CountOfTable = Collection.countOfTable();
   Stream << Collection.name();
   for (int i=0; i < CountOfTable; i++){
      Stream << newline << ' ' << Collection.table(i).name();
   }

   return Stream;
}
