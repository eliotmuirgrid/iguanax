//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABschema
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:15:36 PM
//
//---------------------------------------------------------------------------
#include "NTABprecomp.h"
#pragma hdrstop

#include "NTABschema.h"
#include <COL/COLlog.h>
#include <DB/DBgrammar.h>
COL_LOG_MODULE;

static NTABtype NTABmapType(DBsimpleType Input){
   switch(Input){
      case DBS_STRING:   return NTAB_STRING;
      case DBS_INTEGER:  return NTAB_INTEGER;
      case DBS_DOUBLE:   return NTAB_DOUBLE;
      case DBS_DATETIME: return NTAB_DATETIME;
      default: COLASSERT(1==0);
   }
   return NTAB_STRING;
}

NTABschema::NTABschema(DBcollection* pGrammar){
   COL_METHOD(NTABschema);
   COLmap<COLstring, DBtable>::const_iterator it = pGrammar->Tables.cbegin();
   // Since we dont have a "message" level anymore, we just add one table collection
   NTABcollectionG& Collection = addTableCollection("Tables");
   while (it != pGrammar->Tables.cend()){
      NTABtableG& Table = addTable();
      Table.setName(it->first);
      for (int i = 0; i < it->second.Columns.size(); i++){
         Table.addColumn(it->second.Columns[i].Name, it->second.Columns[i].Description, NTABmapType(it->second.Columns[i].Type), it->second.Columns[i].IsKey);
      }
      Table.index();
      Collection.addTable(it->first);
      it++;
   }
}

NTABschema::~NTABschema(){
   COL_METHOD(~NTABschema);
   clear();
}

const NTABcollectionG* NTABschema::tableCollection(const COLstring& Name) const{
   COL_METHOD(NTABschema::tableCollection);
   COL_VAR(Name);
   COLhashmapPlace i =  m_CollectionTable.find(Name);
   if (i) {
      COL_TRC("Found.");
   } else {
      COL_TRC("Not found.");
   }
   return i != NULL ? m_CollectionTable[i] : NULL;
}

NTABcollectionG* NTABschema::tableCollection(const COLstring& Name){
   COL_VAR(Name);
   COLhashmapPlace i =  m_CollectionTable.find(Name);
   if (i) {
      COL_TRC("Found.");
   } else {
      COL_TRC("Not found.");
   }
   return i != NULL ? m_CollectionTable[i] : NULL;
}

NTABcollectionG& NTABschema::addTableCollection(const COLstring& Name){
   COL_METHOD(NTABschema::addTableCollection);
   COL_VAR(Name);
   COLhashmapPlace i = m_CollectionTable.add(Name, new NTABcollectionG(*this, Name));
   return *m_CollectionTable[i];
}

COLindex NTABschema::firstTableCollection() const{
   return (COLindex)m_CollectionTable.first();
}

COLindex NTABschema::nextTableCollection(COLindex i) const{
   return (COLindex)m_CollectionTable.next((COLhashmapPlace)i);
}

const NTABcollectionG& NTABschema::tableCollection(COLindex i) const{
   return *m_CollectionTable[(COLhashmapPlace)i];  
}

NTABcollectionG& NTABschema::tableCollection(COLindex i){
   return *m_CollectionTable[(COLhashmapPlace)i];
}

int NTABschema::countOfTable() const{
   return m_TableVector.size();
}

const NTABtableG& NTABschema::table(int i) const { return *m_TableVector[i]; }

NTABtableG& NTABschema::table(int i)             { return *m_TableVector[i]; }

NTABtableG& NTABschema::addTable(){
   m_TableVector.push_back(new NTABtableG());
   return *m_TableVector[m_TableVector.size()-1];
}

int NTABschema::tableIndex(const COLstring& Name) const{
   for (int i=0; i < countOfTable(); i++){
      if (table(i).name() == Name){
         return i;
      }
   }
   COL_ERROR_STREAM_PLAIN(Name << " table doesn't exist.", COLerror::PreCondition);
}

void NTABschema::clear(){
   for (COLhashmapPlace i=m_CollectionTable.first(); i != 0; i=m_CollectionTable.next(i)){
      delete m_CollectionTable[i];
   }
   m_CollectionTable.clear();
   m_TableVector.clear();
}

COLostream& operator<<(COLostream& Stream, const NTABschema& Schema){
   COL_FUNCTION(NTABschema<<);
   for (COLindex i= Schema.firstTableCollection(); i != 0; i = Schema.nextTableCollection(i)){
      Stream << Schema.tableCollection(i) << newline;
   }
   COL_TRC("Now output tables.");
   int CountOfTable = Schema.countOfTable();
   for (int j = 0; j < CountOfTable; j++){
      Stream << Schema.table(j) << newline;
   }

   return Stream;
}
