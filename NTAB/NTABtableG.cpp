//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABtableG
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:18:19 PM
//
//---------------------------------------------------------------------------
#include "NTABprecomp.h"
#pragma hdrstop

#include "NTABtableG.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

static const COLstring NTABstring("string");
static const COLstring NTABinteger("integer");
static const COLstring NTABdouble("double");
static const COLstring NTABdatetime("datetime");
void NTABtableGRegisterTypes(){
   COL_FUNCTION(NTABtableGRegisterTypes);
   NODtypeMapAdd(NTABstring,   NTAB_STRING,   NTABprotocolName, NTAB_TABLE_PROTOCOL);
   NODtypeMapAdd(NTABinteger,  NTAB_INTEGER,  NTABprotocolName, NTAB_TABLE_PROTOCOL);
   NODtypeMapAdd(NTABdouble,   NTAB_DOUBLE,   NTABprotocolName, NTAB_TABLE_PROTOCOL);
   NODtypeMapAdd(NTABdatetime, NTAB_DATETIME, NTABprotocolName, NTAB_TABLE_PROTOCOL);
}

const COLstring& NTABtypeName(NTABtype Type){
   switch(Type){
   case NTAB_STRING:   return NTABstring;
   case NTAB_INTEGER:  return NTABinteger;
   case NTAB_DOUBLE:   return NTABdouble;
   case NTAB_DATETIME: return NTABdatetime;
   default: COLASSERT(1==0);
   }
   return NTABstring;
}

const COLstring& NTABcolumn::typeName() const{
   return NTABtypeName(m_Type);
}

NTABtableG::NTABtableG() : m_CountOfRef(0) {
   COL_METHOD(NTABtableG::NTABtableG);
}

NTABtableG::~NTABtableG(){
   COL_METHOD(NTABtableG::~NTABtableG);
}

NTABtableG::NTABtableG(const NTABtableG& Orig) : m_CountOfRef(0){
   COL_METHOD(NTABtableG-copy);
   operator=(Orig);
}

NTABtableG& NTABtableG::operator=(const NTABtableG& Orig){
   COL_METHOD(operator=);
   m_ColumnVector = Orig.m_ColumnVector;
   m_Name = Orig.m_Name;
   m_RowName = Orig.m_RowName;
   return *this;
}

void NTABtableG::setName(const COLstring& Name){
   m_Name = Name;
   m_RowName = Name  + " Row";
}

int NTABtableG::countOfColumn() const{
   return m_ColumnVector.size();
}

const NTABcolumn& NTABtableG::column(int i) const{
   return m_ColumnVector[i];
}

NTABcolumn& NTABtableG::column(int i){
   return m_ColumnVector[i];
}

void NTABtableG::addColumn(const COLstring& Name, const COLstring& Description, NTABtype Type, bool IsKey){
   m_ColumnVector.push_back(NTABcolumn(Name, Description, Type, IsKey));
}

void NTABtableG::removeColumn(int i){
   m_ColumnVector.remove(i);
   m_ColumnIndex.clear();
   index();
}

void NTABtableG::index(){
   int C = countOfColumn();
   for (int i=0; i < C; i++){
      m_ColumnIndex.add(column(i).name(),i);
   }
}

int NTABtableG::columnIndex(const COLstring& Name) const{
   COLhashmapPlace i = m_ColumnIndex.find(Name);
   return i != 0 ? m_ColumnIndex[i] : -1;
}

void NTABtableG::Release(){
   COLASSERT(m_CountOfRef > 0);
   m_CountOfRef--;
   if (0 == m_CountOfRef)
   {
      delete this;  // good bye cruel world...!
   }
}

COLostream& operator<<(COLostream& Stream, const NTABtableG& Table){
   Stream << "Table: " << Table.name() << newline;
   int CountOfColumn = Table.countOfColumn();
   for (int i=0; i < CountOfColumn; i++){
      Stream << ' ' << i << COL_T(") ") << Table.column(i).name() << '(' << Table.column(i).typeName() << ')' << newline;
   }
   return Stream;
}
