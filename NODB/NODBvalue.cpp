//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODBvalue
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Thursday, June 17th, 2010 @ 03:18:17 PM
//
//---------------------------------------------------------------------------
#include "NODBprecomp.h"
#pragma hdrstop

#include "NODBvalue.h"

#include <DB/DBresultSet.h>
#include <DB/DBvariant.h>

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

NODBvalue::NODBvalue(DBresultSet* pResultSet, int Row, int Column) : m_pResultSet(pResultSet), m_Row(Row), m_Column(Column) {
   COL_METHOD(NODBvalue::NODBvalue);
   if (!isNull()){
      // Cache the value.
      const DBvariant& ColumnValue = m_pResultSet->value(m_Row, m_Column);
      if (ColumnValue.dataType() == DB_DATETIME){
         const COLdateTime NODB_EMPTY_DATETIME(0, 0, 0, 0, 0, 0);
         const COLstring   NODB_EMPTY_DATETIME_STRING("0000-00-00 00:00:00");
         const COLdateTime& ValueAsDateTime = ColumnValue.dateTime();
         if (ValueAsDateTime == NODB_EMPTY_DATETIME){
            m_CachedValue = NODB_EMPTY_DATETIME_STRING;
         } else {
            m_CachedValue = ValueAsDateTime.format("%Y-%m-%d %H:%M:%S");
         }
      } else {
         m_CachedValue = ColumnValue.toString();
      }
   }
}

int NODBvalue::nodeType() const {
   COL_METHOD(NODBvalue::nodeType);
   switch (m_pResultSet->columnType(m_Column))
   {
   case DB_INTEGER:
   case DB_LARGE_INTEGER:
   case DB_BOOLEAN:
      return NODB_VALUE_INTEGER;
   case DB_DOUBLE:
   case DB_LARGE_DOUBLE:
      return NODB_VALUE_DOUBLE;
   case DB_DATETIME:
      return NODB_VALUE_DATETIME;
   case DB_BINARY:
      return NODB_VALUE_BINARY;
   default:
      return NODB_VALUE_STRING;
   }
}

static const COLstring NODBvalueString("string"); 
static const COLstring NODBvalueInteger("integer"); 
static const COLstring NODBvalueDouble("double"); 
static const COLstring NODBvalueDateTime("datetime"); 
static const COLstring NODBvalueBinary("binary"); 

void NODBvalueRegisterTypes(){
   COL_FUNCTION(NODBvalueRegisterTypes);
   NODtypeMapAdd(NODBvalueString, NODB_VALUE_STRING,     NODBprotocolName, NODB_DB);
   NODtypeMapAdd(NODBvalueInteger, NODB_VALUE_INTEGER,   NODBprotocolName, NODB_DB);
   NODtypeMapAdd(NODBvalueDouble, NODB_VALUE_DOUBLE,     NODBprotocolName, NODB_DB);
   NODtypeMapAdd(NODBvalueDateTime, NODB_VALUE_DATETIME, NODBprotocolName, NODB_DB);
   NODtypeMapAdd(NODBvalueBinary, NODB_VALUE_BINARY,     NODBprotocolName, NODB_DB);
}

const COLstring& NODBvalue::nodeTypeName() const
{
   COL_METHOD(NODBvalue::nodeTypeName);
   switch(nodeType())
   {
   case NODB_VALUE_STRING:   return NODBvalueString; 
   case NODB_VALUE_INTEGER:  return NODBvalueInteger;
   case NODB_VALUE_DOUBLE:   return NODBvalueDouble;
   case NODB_VALUE_DATETIME: return NODBvalueDateTime;
   case NODB_VALUE_BINARY:   return NODBvalueBinary;
   default:
      COLASSERT(false);
      return NODBvalueString;
   }
}

const COLstring& NODBvalue::name() const
{
   COL_METHOD(NODBvalue::name);
   return m_pResultSet->columnName(m_Column);
}

bool NODBvalue::isNull() const{
   if (m_Row == -1){
      return true;
   } else {
      const DBvariant& ColumnValue = m_pResultSet->value(m_Row, m_Column);
      // Don't use ColumnValue.isNull() - that tells you if it is an empty string or 0.
      return ColumnValue.dataType() == DB_DATA_TYPE_NOT_DEFINED;
   }
}
