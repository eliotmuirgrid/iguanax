//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NTABtableItem
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, October 22nd, 2010 @ 12:27:09 PM
//
//---------------------------------------------------------------------------
#include "NTABprecomp.h"
#pragma hdrstop

#include "NTABtableItem.h"
#include "NTABtableG.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

NTABtableItem::NTABtableItem(NTABtableG* pGrammar, int Column)
 : m_pGrammar(pGrammar),
   m_Column(Column),
   m_IsNull(true)
{
   COL_METHOD(NTABtableItem::NTABtableItem);
   COL_VAR3(m_pGrammar.get(), m_Column, m_IsNull);
}

NTABtableItem::~NTABtableItem(){
}

const COLstring& NTABtableItem::value() const { 
   COL_FUNCTION(NTABtableItem::value);
   return m_Value; }

int NTABtableItem::nodeType() const {
   COL_FUNCTION(NTABtableItem::nodeType);
   return m_pGrammar->column(m_Column).type();
}

const COLstring& NTABtableItem::nodeTypeName() const{
   COL_FUNCTION(NTABtableItem::nodeTypeName);
   return m_pGrammar->column(m_Column).typeName();  
}

const COLstring& NTABtableItem::name() const{
   COL_FUNCTION(NTABtableItem::name);
   return m_pGrammar->column(m_Column).name();
}

bool NTABtableItem::isKey() const {
   COL_FUNCTION(NTABtableItem::isKey);
   COL_METHOD(NTABtableItem::isKey);
   if(m_pGrammar.get()){
      COL_VAR(m_pGrammar->column(m_Column).name());
      return m_pGrammar->column(m_Column).isKey();
   }
   COL_TRC("No grammar so returning false.");
   return false;
}
void NTABtableItem::setIsNull(bool IsNull){
   COL_FUNCTION(NTABtableItem::setIsNull);
   if (!m_IsNull && IsNull){
      m_Value.clear();
   }
   m_IsNull = IsNull;
}

NTABdateTimeItem::NTABdateTimeItem(NTABtableG* pGrammar, int Column) : NTABtableItem(pGrammar, Column) {}
NTABintItem::NTABintItem          (NTABtableG* pGrammar, int Column) : NTABtableItem(pGrammar, Column) {}
NTABdoubleItem::NTABdoubleItem    (NTABtableG* pGrammar, int Column) : NTABtableItem(pGrammar, Column) {}
NTABstringItem::NTABstringItem    (NTABtableG* pGrammar, int Column) : NTABtableItem(pGrammar, Column) {}

void NTABdateTimeItem::setValue(const COLstring& Value){
   COL_FUNCTION(NTABdateTimeItem::setValue);
   if (Value.is_null()){
      setIsNull(true);
      m_Value.clear();
      return;
   }
   int nYear=0, nMonth=0, nDay=0, nHour=0, nMin=0, nSec=0;
   int ReturnCode = sscanf(Value.c_str(), "%04d-%02d-%02d %02d:%02d:%02d", &nYear, &nMonth, &nDay,  &nHour, &nMin, &nSec);
   if (3 != ReturnCode && 6 != ReturnCode){
      COL_ERROR_STREAM_PLAIN(COL_T("Couldn't parse date '") << Value << COL_T("'.  Required format is YYYY-MM-DD or YYYY-MM-DD HH:MM:SS"),
         COLerror::PreCondition);  // TODO - should we define a specific error code here...
   }
   COLdateTime Validator;
   Validator.setDateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
   if (Validator.status() != COLdateTime::valid){
      COL_ERROR_STREAM_PLAIN('\'' << Value << COL_T("' is not a valid date."), COLerror::PreCondition);
   }
   m_Value = Value; //m_Value.format("%Y-%m-%d %H:%M:%S");
   if (isNull()) setIsNull(false);
}

void NTABstringItem::setValue(const COLstring& Value) {
   COL_METHOD(NTABstringItem::setValue);
   m_Value = Value;
   if (isNull()) setIsNull(false);
}

void NTABintItem::setValue(const COLstring& Value){
   COL_METHOD(NTABintItem::setValue);
   int Number = 0;
   if (!Value.is_null()){
      Number = atoi(Value.c_str());
   }
   m_Value = COLintToString(Number);
   if (isNull()) setIsNull(false);
}

void NTABdoubleItem::setValue(const COLstring& Value){  // TODO - this is probably where we get burned - doubles are hard to store...
   COL_METHOD(NTABdoubleItem::setValue);
   double Number = 0;
   if (!Value.is_null()){
      Number = atof(Value.c_str());
   }
   m_Value.clear();
   COLostream Stream(m_Value);
   Stream << Number;
   if (isNull()) setIsNull(false);
}

NODplace* NTABnewTableItem(NTABtableG* pGrammar, int Column){
   COL_FUNCTION(NTABnewTableItem);
   switch(pGrammar->column(Column).type()){
   case NTAB_STRING:   return new NTABstringItem  (pGrammar, Column);
   case NTAB_DATETIME: return new NTABdateTimeItem(pGrammar, Column);
   case NTAB_INTEGER:  return new NTABintItem     (pGrammar, Column);
   case NTAB_DOUBLE:   return new NTABdoubleItem  (pGrammar, Column);
   }
   COLASSERT(false);
   return NULL;
}
