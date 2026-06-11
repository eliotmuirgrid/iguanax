//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBvariant
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, December 28th, 2007 @ 11:54:09 PM
//
//---------------------------------------------------------------------------
#include "DBprecomp.h"
#pragma hdrstop

#include "DBvariant.h"
#include "DButils.h"

#include <COL/COLdateTime.h>
#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <stdio.h>

// NOTE : On Windows, Visual Studio 2013 or lower, snprintf is not directly available - have to use _snprintf
#if defined(_WIN32)
#define snprintf _snprintf
#endif

static void DBvariantTraceType(DBdataType DataType, const COLstring& VarName = "DataType") {
   // no COL_FUNCTION trace - too noisy
   switch (DataType) {
#define __STRINGIFY_CASE(casenum)   case casenum: COL_TRC(VarName << " = " #casenum); break
      __STRINGIFY_CASE(DB_DATA_TYPE_NOT_DEFINED);
      __STRINGIFY_CASE(DB_STRING);
      __STRINGIFY_CASE(DB_TEXT);
      __STRINGIFY_CASE(DB_INTEGER);
      __STRINGIFY_CASE(DB_DOUBLE);
      __STRINGIFY_CASE(DB_DATETIME);
      __STRINGIFY_CASE(DB_LARGE_INTEGER);
      __STRINGIFY_CASE(DB_LARGE_DOUBLE);
      __STRINGIFY_CASE(DB_BOOLEAN);
      __STRINGIFY_CASE(DB_BINARY);
#undef __STRINGIFY_CASE
   default:
      COL_WRN("Unknown type");
   }
}

DBvariant::DBvariant()
: DataType(DB_DATA_TYPE_NOT_DEFINED)
{
   COL_METHOD(DBvariant::DBvariant);
   Value.Integer = 0;
}

DBvariant::DBvariant(COLint32 Integer)
: DataType(DB_INTEGER)
{
   COL_METHOD(DBvariant::DBvariant-Integer);
   Value.Integer = Integer;
}

DBvariant::DBvariant(COLint64 LargeInteger)
: DataType(DB_LARGE_INTEGER)
{
   COL_METHOD(DBvariant::DBvariant-LargeInteger);
   Value.pLargeInteger = new COLint64(LargeInteger);
}

DBvariant::DBvariant(COLfloat32 Float)
: DataType(DB_DOUBLE)
{
   COL_METHOD(DBvariant::DBvariant-Float);
   Value.Float = Float;
}

DBvariant::DBvariant(COLfloat64 Float)
: DataType(DB_LARGE_DOUBLE)
{
   COL_METHOD(DBvariant::DBvariant-Float64);
   Value.pLargeFloat = new COLfloat64(Float);
}

DBvariant::DBvariant(const COLstring& String)
: DataType(DB_STRING)
{
   COL_METHOD(DBvariant::DBvariant-String);
   Value.pString = new COLstring(String);
}

DBvariant::DBvariant(const char* pString)
: DataType(DB_STRING)
{
   COL_METHOD(DBvariant::DBvariant-pString);
   Value.pString = new COLstring(pString);
}

DBvariant::DBvariant(const COLstring& Buffer, DBdataType DatabaseType) // We ignore the datatype
: DataType(DB_BINARY)
{
   COL_METHOD(DBvariant::DBvariant-Buffer);
   Value.pString = new COLstring(Buffer);
}


DBvariant::DBvariant(const COLdateTime& DateTime)
: DataType(DB_DATETIME)
{
   COL_METHOD(DBvariant::DBvariant-DateTime);
   Value.pDateTime = new COLdateTime(DateTime);
}

DBvariant::DBvariant(bool NewValue)
: DataType(DB_BOOLEAN)
{
   COL_METHOD(DBvariant::DBvariant-Bool);
   Value.Boolean = NewValue;
}

DBvariant::DBvariant(const DBvariant& Orig)
: DataType(Orig.DataType)
{
   COL_METHOD(DBvariant::DBvariant-CopyCtor);
   DBvariantTraceType(DataType);
   switch (DataType)
   {
   case DB_DATA_TYPE_NOT_DEFINED:
      Value.Integer = 0;
      break;
   case DB_STRING:
      Value.pString = new COLstring(*Orig.Value.pString);
      break;
   case DB_BINARY:
      Value.pString = new COLstring(*Orig.Value.pString);
      break;
   case DB_INTEGER:
      Value.Integer = Orig.Value.Integer;
      break;
   case DB_LARGE_INTEGER:
      Value.pLargeInteger = new COLint64(*Orig.Value.pLargeInteger);
      break;
   case DB_DOUBLE:
      Value.Float = Orig.Value.Float;
      break;
   case DB_LARGE_DOUBLE:
      Value.pLargeFloat = new COLfloat64(*Orig.Value.pLargeFloat);
      break;
   case DB_DATETIME:
      Value.pDateTime = new COLdateTime(*Orig.Value.pDateTime);
      break;
   case DB_BOOLEAN:
      Value.Boolean = Orig.Value.Boolean;
      break;
   default:
      COL_ERROR_STREAM("Unknown type.", COLerror::PreCondition);
   }
}

DBvariant& DBvariant::operator=(const DBvariant& Orig)
{
   COL_METHOD(DBvariant::operator=);
   if (this != &Orig)
   {
      cleanUpValue();
      DataType = Orig.DataType;
      DBvariantTraceType(DataType);
      switch (DataType)
      {
      case DB_DATA_TYPE_NOT_DEFINED:
         Value.Integer = 0;
         break;
      case DB_STRING:
         Value.pString = new COLstring(*Orig.Value.pString);
         break;
      case DB_BINARY:
         Value.pString = new COLstring(*Orig.Value.pString);
         break;
      case DB_INTEGER:
         Value.Integer = Orig.Value.Integer;
         break;
      case DB_LARGE_INTEGER:
         Value.pLargeInteger = new COLint64(*Orig.Value.pLargeInteger);
         break;
      case DB_DOUBLE:
         Value.Float = Orig.Value.Float;
         break;
      case DB_LARGE_DOUBLE:
         Value.pLargeFloat = new COLfloat64(*Orig.Value.pLargeFloat);
         break;
      case DB_DATETIME:
         Value.pDateTime = new COLdateTime(*Orig.Value.pDateTime);
         break;
      case DB_BOOLEAN:
         Value.Boolean = Orig.Value.Boolean;
         break;
      default:
         COL_ERROR_STREAM("Unknown type.", COLerror::PreCondition);
      }
   }
   return *this;
}

void DBvariant::cleanUpValue()
{
   COL_METHOD(DBvariant::cleanUpValue);
   DBvariantTraceType(DataType);
   switch (DataType)
   {
   case DB_STRING:
      delete Value.pString;
      break;
   case DB_BINARY:
      delete Value.pString;
      break;
   case DB_DATETIME:
      delete Value.pDateTime;
      break;
   case DB_LARGE_INTEGER:
      delete Value.pLargeInteger;
      break;
   case DB_LARGE_DOUBLE:
      delete Value.pLargeFloat;
      break;
   case DB_DATA_TYPE_NOT_DEFINED:
   case DB_INTEGER:
   case DB_DOUBLE:
   case DB_BOOLEAN:
      COL_TRC("Doing nothing for DB_DATA_TYPE_NOT_DEFINED, DB_INTEGER, DB_DOUBLE, DB_BOOLEAN");
      // do nothing;
      break;
   default:
      COL_ERROR_STREAM(COL_T("Unknown type ") << DataType, COLerror::PreCondition);
   }
}

DBvariant::~DBvariant()
{
   COL_METHOD(DBvariant::~DBvariant);
   cleanUpValue();
}

COLstring DBstringFromDateTime(const COLdateTime& DateTime)
{
   COL_FUNCTION(DBstringFromDateTime);
   COL_VAR(DateTime);
   if (DateTime.isNull())
   {
      COL_TRC("Date time is null");
      return "<null>";
   }
   else if (DateTime.status() == COLdateTime::invalid)
   {
      COL_TRC("Invalid datetime!");
      return "(invalid)";
   }
   else
   {
      COL_TRC("Running snprintf to format datetime to: DD/MM/YYYY HH:MM:SS");
      // no datetime string representation should be longer than this...
      static const int DatetimeBufferSize = 200;
      char Buffer[DatetimeBufferSize];
      int BufferLen =
         snprintf(Buffer, DatetimeBufferSize,
                  "%02i/%02i/%04i %02i:%02i:%02i",
                  DateTime.day(),
                  DateTime.month(),
                  DateTime.year(),
                  DateTime.hour(),
                  DateTime.minute(),
                  DateTime.second()
                  );
      COLASSERT(BufferLen != DatetimeBufferSize);  // if this happens, increase buffer size

      COLstring Result = COLstring(Buffer, BufferLen);
      COL_VAR(Result);
      return Result;
   }
}


COLstring DBintegerToString(COLint32 Integer)
{
   COL_FUNCTION(DBintegerToString);
   COL_VAR(Integer);
   COLstring ReturnString;
   COLostream ReturnStream( ReturnString );
   ReturnStream << Integer;
   return ReturnString;
}

COLstring DBlargeIntegerToString(COLint64 Integer)
{
   COL_FUNCTION(DBlargeIntegerToString);
   COL_VAR(Integer);
   COLstring ReturnString;
   COLostream ReturnStream( ReturnString );
   ReturnStream << Integer;
   return ReturnString;
}

COLstring DBfloatToString(COLfloat32 Value)
{
   COL_FUNCTION(DBfloatToString);
   COL_VAR(Value);
   COLstring ReturnString;
   COLostream ReturnStream( ReturnString );
   ReturnStream << Value;
   return ReturnString;
}

COLstring DBlargeFloatToString(COLfloat64 Value)
{
   COL_FUNCTION(DBlargeFloatToString);
   COL_VAR(Value);
   COLstring ReturnString;
   COLostream ReturnStream( ReturnString );
   ReturnStream << Value;
   return ReturnString;
}

COLstring DBbooleanToString(bool Value)
{
   COL_FUNCTION(DBbooleanToString);
   COL_VAR(Value);
   if (Value)
   {
      return "true";
   }
   else
   {
      return "false";
   }
}

COLstring DBvariant::toString() const
{
   COL_METHOD(DBvariant::toString);
   DBvariantTraceType(DataType);
   switch(DataType)
   {
   case DB_DATA_TYPE_NOT_DEFINED:
      return "";
      break;
   case DB_STRING:
      return *Value.pString;
      break;
   case DB_BINARY:
      return *Value.pString;
      break;
   case DB_INTEGER:
      return DBintegerToString(Value.Integer);
      break;
   case DB_LARGE_INTEGER:
      return DBlargeIntegerToString(*Value.pLargeInteger);
      break;
   case DB_DOUBLE:
      return DBfloatToString(Value.Float);
      break;
   case DB_LARGE_DOUBLE:
      return DBlargeFloatToString(*Value.pLargeFloat);
      break;
   case DB_DATETIME:
      return DBstringFromDateTime(*Value.pDateTime);
      break;
   case DB_BOOLEAN:
      return DBbooleanToString(Value.Boolean);
      break;
   default:
      COL_ERROR_STREAM("Unknown type.", COLerror::PreCondition);
   }
}

const char* DBvariantTypeName(DBdataType DataType)
{
   COL_FUNCTION(DBvariantTypeName);
   DBvariantTraceType(DataType);
   switch(DataType)
   {
   case DB_DATA_TYPE_NOT_DEFINED:
      return "null";
      break;
   case DB_STRING:
      return "string";
      break;
   case DB_INTEGER:
      return "int32";
      break;
   case DB_LARGE_INTEGER:
      return "int64";
      break;
   case DB_DOUBLE:
      return "float32";
      break;
   case DB_LARGE_DOUBLE:
      return "float64";
      break;
   case DB_DATETIME:
      return "date/time";
      break;
   case DB_BOOLEAN:
      return "boolean";
      break;
   default:
      COL_ERROR_STREAM("Unknown type.", COLerror::PreCondition);
   }
}

void DBvariant::initType()
{
   COL_METHOD(DBvariant::initType);
   DBvariantTraceType(DataType);
   switch(DataType)
   {
   case DB_STRING:
      Value.pString = new COLstring();
      break;
   case DB_INTEGER:
      Value.Integer = 0;
      break;
   case DB_LARGE_INTEGER:
      Value.pLargeInteger = new COLint64(0);
      break;
   case DB_DOUBLE:
      Value.Float = 0.0;
      break;
   case DB_LARGE_DOUBLE:
      Value.pLargeFloat = new COLfloat64(0.0);
      break;
   case DB_DATETIME:
      Value.pDateTime = new COLdateTime();
      break;
   case DB_BOOLEAN:
      Value.Boolean = false;
      break;
   case DB_BINARY:
      Value.pString = new COLstring();
      break;
   case DB_DATA_TYPE_NOT_DEFINED:
      COL_ERROR_STREAM("Null types should not be initialized", COLerror::PreCondition);
   default:
      COL_ERROR_STREAM("Unknown type.", COLerror::PreCondition);
   }
}

void DBvariant::verifyType(DBdataType ExpectedType) const
{
   COL_METHOD(DBvariant::verifyType);
   DBvariantTraceType(DataType);
   DBvariantTraceType(ExpectedType, "ExpectedType");
   if (DataType != ExpectedType)
   {
      if (DataType == DB_DATA_TYPE_NOT_DEFINED)
      {
         COL_TRC("Undefined - changing type to match expected");
         ((DBvariant*)this)->DataType = ExpectedType;  // a little dubious that this 'const' operation alters the object
         ((DBvariant*)this)->initType();
      }
      else
      {
         COLstring Message("Operation only valid on ");
         Message += DBvariantTypeName(ExpectedType);
         Message += " variants.";
         throw COLerror(Message, COLerror::PreCondition);
      }
   }
}

COLstring& DBvariant::string()
{
   COL_METHOD(DBvariant::string);
   verifyType(DB_STRING);
   return *Value.pString;
}

const COLstring& DBvariant::string() const
{
   COL_METHOD(DBvariant::string);
   verifyType(DB_STRING);
   return *Value.pString;
}

bool& DBvariant::boolean()
{
   COL_METHOD(DBvariant::boolean);
   verifyType(DB_BOOLEAN);
   return Value.Boolean;
}

const bool& DBvariant::boolean() const
{
   COL_METHOD(DBvariant::boolean);
   verifyType(DB_BOOLEAN);
   return Value.Boolean;
}

COLint32& DBvariant::integer32()
{
   COL_METHOD(DBvariant::integer32);
   verifyType(DB_INTEGER);
   return Value.Integer;
}

const COLint32& DBvariant::integer32() const
{
   COL_METHOD(DBvariant::integer32);
   verifyType(DB_INTEGER);
   return Value.Integer;
}

COLint64& DBvariant::integer64()
{
   COL_METHOD(DBvariant::integer64);
   verifyType(DB_LARGE_INTEGER);
   return *Value.pLargeInteger;
}

const COLint64& DBvariant::integer64() const
{
   COL_METHOD(DBvariant::integer64);
   verifyType(DB_LARGE_INTEGER);
   return *Value.pLargeInteger;
}

COLfloat32& DBvariant::float32()
{
   COL_METHOD(DBvariant::float32);
   verifyType(DB_DOUBLE);
   return Value.Float;
}

const COLfloat32& DBvariant::float32() const
{
   COL_METHOD(DBvariant::float32);
   verifyType(DB_DOUBLE);
   return Value.Float;
}

COLfloat64& DBvariant::float64()
{
   COL_METHOD(DBvariant::float64);
   verifyType(DB_LARGE_DOUBLE);
   return *Value.pLargeFloat;
}

const COLfloat64& DBvariant::float64() const
{
   COL_METHOD(DBvariant::float64);
   verifyType(DB_LARGE_DOUBLE);
   return *Value.pLargeFloat;
}

COLdateTime& DBvariant::dateTime()
{
   COL_METHOD(DBvariant::dateTime);
   verifyType(DB_DATETIME);
   return *Value.pDateTime;
}

const COLdateTime& DBvariant::dateTime() const
{
   COL_METHOD(DBvariant::dateTime);
   verifyType(DB_DATETIME);
   return *Value.pDateTime;
}

const COLstring& DBvariant::binary() const
{
   COL_METHOD(DBvariant::binary);
   verifyType(DB_BINARY);
   return *Value.pString;
}

COLstring& DBvariant::binary()
{
   COL_METHOD(DBvariant::binary);
   verifyType(DB_BINARY);
   return *Value.pString;
}

COLstring DBdataTypeName(DBdataType DataType)
{
   COL_FUNCTION(DBdataTypeName);
   DBvariantTraceType(DataType);
   switch(DataType)
   {
   case DB_DATA_TYPE_NOT_DEFINED:
      return "Null";
      break;
   case DB_STRING:
      return "String";
      break;
   case DB_INTEGER:
      return "32 Bit Integer";
      break;
   case DB_LARGE_INTEGER:
      return "64 Bit Integer";
      break;
   case DB_DOUBLE:
      return "32 Bit Float";
      break;
   case DB_LARGE_DOUBLE:
      return "64 Bit Float";
      break;
   case DB_DATETIME:
      return "Date Time";
      break;
   case DB_BOOLEAN:
      return "Boolean";
      break;
   default:
      return "Unknown Type";
   }
}

bool DBvariant::isNull() const
{
   COL_METHOD(DBvariant::isNull);
   DBvariantTraceType(DataType);
   switch(DataType)
   {
   case DB_DATA_TYPE_NOT_DEFINED:
      return true;
      break;
   case DB_STRING:
      return Value.pString->is_null();
      break;
   case DB_INTEGER:
      return (Value.Integer == 0);
      break;
   case DB_LARGE_INTEGER:
      return (*Value.pLargeInteger == 0);
      break;
   case DB_DOUBLE:
      return (Value.Float == (COLfloat32)0.0);
      break;
   case DB_LARGE_DOUBLE:
      return (*Value.pLargeFloat == (COLfloat64)0.0);
      break;
   case DB_DATETIME:
      return (*Value.pDateTime == COLdateTime());
      break;
   case DB_BOOLEAN:  // false is NULL - rather arbitrary
      return !Value.Boolean;
      break;
   case DB_BINARY:
      return Value.pString->size()==0;
      break;
   default:
      COL_ERROR_STREAM("Unknown type for testing null", COLerror::PreCondition);
   }
}

COLuint32 DBvariant::binarySize() const
{
   COL_METHOD(DBvariant::binarySize);
   DBvariantTraceType(DataType);
   switch(DataType)
   {
   case DB_DATA_TYPE_NOT_DEFINED:
      return 0;
      break;
   case DB_STRING:
      return Value.pString->size();
      break;
   case DB_INTEGER:
      return 4;
      break;
   case DB_LARGE_INTEGER:
      return 8;
      break;
   case DB_DOUBLE:
      return 4;
      break;
   case DB_LARGE_DOUBLE:
      return 8;
      break;
   case DB_DATETIME:
      return 8;
      break;
   case DB_BOOLEAN:
      return 4;
   case DB_BINARY:
      return Value.pString->size();
      break;
   default:
      COL_ERROR_STREAM("Unknown type", COLerror::PreCondition);
   }
}

// Pretty loose code here...
DBvariant DBvariantFromString(const COLstring& String, DBdataType DataType)
{
   COL_FUNCTION(DBvariantFromString);
   COL_VAR(String);
   DBvariantTraceType(DataType);
   switch(DataType)
   {
   case DB_DATA_TYPE_NOT_DEFINED:
      return DBvariant();
      break;
   case DB_STRING: return DBvariant(String);
   case DB_INTEGER:
      {
         COLint32 Integer = atoi(String.c_str());
         return DBvariant(Integer);
      }
      break;
   case DB_LARGE_INTEGER:
      {
         COLint64 Integer = COLmath::atoi64(String.c_str());
         return DBvariant(Integer);
      }
      break;
   case DB_DOUBLE:
      {
         COLfloat32 Float = (float)atof(String.c_str());
         return DBvariant(Float);
      }
      break;
   case DB_LARGE_DOUBLE:
      {
         COLfloat64 Float = atof(String.c_str());
         return DBvariant(Float);
      }
      break;
   case DB_BOOLEAN:
      {
         if (String == "true")
         {
            return DBvariant(true);
         }
         else
         {
            return DBvariant(false);
         }
      }
      break;
   case DB_BINARY: { return DBvariant(String, DB_BINARY); }
   case DB_DATETIME:
      COL_ERROR_STREAM("Converting strings to date time DBvariant not supported.", COLerror::PreCondition);
   default:
      COL_ERROR_STREAM("Unknown type", COLerror::PreCondition);
   }
}


//returns true if the two variants are equal
bool DBvariant::operator==(const DBvariant& Rhs) const
{
   COL_METHOD(DBvariant::operator==);
   DBvariantTraceType(DataType);
   DBvariantTraceType(Rhs.DataType, "Rhs.DataType");
   // TODO should really test if my casts are safe .... Eliot
   // I'm not bothering to compare floats and integers
   switch(DataType)
   {
   case DB_DATA_TYPE_NOT_DEFINED:
      return (Rhs.DataType == DB_DATA_TYPE_NOT_DEFINED);  // TODO more logic required here
      break;
   case DB_STRING:
      {
         // TODO - if we wanted to get pedantic we could convert to a string to an integer etc.
         if (Rhs.DataType != DB_STRING)
         {
            return false;
         }
         return (Value.pString->compare(*Rhs.Value.pString) == 0);
      }
      break;
   case DB_INTEGER:
      switch(Rhs.DataType)
      {
      case DB_INTEGER:
         return (Value.Integer == Rhs.Value.Integer);
      case DB_LARGE_INTEGER:
         return ((COLint64)Value.Integer == *Rhs.Value.pLargeInteger);
      case DB_DOUBLE:
         return ((COLfloat32)Value.Integer == Rhs.Value.Float);
      case DB_LARGE_DOUBLE:
         return ((COLfloat64)Value.Integer == *Rhs.Value.pLargeFloat);
      default:
         return false;
      }
      break;
   case DB_LARGE_INTEGER:
      switch(Rhs.DataType)
      {
      case DB_INTEGER:
         return (*Value.pLargeInteger == (COLint64)Rhs.Value.Integer);
      case DB_LARGE_INTEGER:
         return (*Value.pLargeInteger == *Rhs.Value.pLargeInteger);
      case DB_DOUBLE:
         return ((COLfloat32)*Value.pLargeInteger == Rhs.Value.Float);
      case DB_LARGE_DOUBLE:
         return ((COLfloat64)*Value.pLargeInteger == *Rhs.Value.pLargeFloat);
      default:
         return false;
      }
      break;
   case DB_DOUBLE:
      switch(Rhs.DataType)
      {
      case DB_DOUBLE:
         return (Value.Float == Rhs.Value.Float);
      case DB_LARGE_DOUBLE:
         return ((COLfloat64)Value.Float == *Rhs.Value.pLargeFloat);
      case DB_INTEGER:
         return (Value.Float == (COLfloat32)Rhs.Value.Integer);
      case DB_LARGE_INTEGER:
         return (Value.Float == (COLfloat32)*Rhs.Value.pLargeInteger);
      default:
         return false;
      }
      break;
   case DB_LARGE_DOUBLE:
      switch(Rhs.DataType)
      {
      case DB_DOUBLE:
         return (*Value.pLargeFloat == (COLfloat64)Rhs.Value.Float);
      case DB_LARGE_DOUBLE:
         return (*Value.pLargeFloat == *Rhs.Value.pLargeFloat);
      case DB_INTEGER:
         return (*Value.pLargeFloat == (COLfloat64)Rhs.Value.Integer);
      case DB_LARGE_INTEGER:
         return (*Value.pLargeFloat == (COLfloat64)*Rhs.Value.pLargeInteger);
      default:
         return false;
      }
      break;
   case DB_DATETIME:
      {
         if (Rhs.DataType != DB_DATETIME)
         {
            return false;  // TODO - we could try and convert strings and doubles into dates....
         }
         return (*Value.pDateTime == *Rhs.Value.pDateTime);
      }
      break;
   case DB_BOOLEAN:
      switch(Rhs.DataType)
      {
      case DB_INTEGER:
         return ((COLint32)Value.Boolean == Value.Integer);
      case DB_LARGE_INTEGER:
         return ((COLint64)Value.Boolean == *Value.pLargeInteger);
      case DB_BOOLEAN:
         return (Value.Boolean == Rhs.Value.Boolean);
      default:
         return false;
      }
      break;
   case DB_BINARY:
      return *Value.pString == *Rhs.Value.pString;
      break;
   default:
      COL_ERROR_STREAM("Unknown type", COLerror::PreCondition);
   }
}

COLstring DBvariantDebugString(const DBvariant& Var){
   COL_FUNCTION(DBvariantDebugString);
   DBvariantTraceType(Var.dataType(), "Var.DataType");
   COLstring Out;
   COLostream Stream(Out);
   switch(Var.dataType())
   {
   case DB_DATA_TYPE_NOT_DEFINED:
      Stream << COL_T("NULL");
      break;
   case DB_STRING:
      Stream << Var.string();
      break;
   case DB_INTEGER:
      Stream << Var.integer32();
      break;
   case DB_LARGE_INTEGER:
      Stream << Var.integer64();
      break;
   case DB_DOUBLE:
      Stream << Var.float32();
      break;
   case DB_LARGE_DOUBLE:
      Stream << Var.float64();
      break;
   case DB_DATETIME:
      Stream << Var.dateTime();
      break;
   case DB_BOOLEAN:
      Stream << Var.boolean();
      break;
   case DB_BINARY:
      Stream  << "0x";
      DBbinaryToHex(*Stream.sink(),Var.binary().data(),Var.binary().size());
      break;
   default:
      COL_ERROR_STREAM("Unknown type", COLerror::PreCondition);
   }
   return Out;
}

COLint64 DBvariant::toInteger() const
{
   COL_METHOD(DBvariant::toInteger);
   DBvariantTraceType(DataType);
   switch(DataType) {
   case DB_INTEGER:       return (COLint64)integer32();
   case DB_LARGE_INTEGER: return (COLint64)integer64();
   case DB_DOUBLE:        return (COLint64)float32();
   case DB_LARGE_DOUBLE:  return (COLint64)float64();
   case DB_NULL:
   case DB_DATA_TYPE_NOT_DEFINED:
   case DB_BOOLEAN:
   case DB_STRING:
   case DB_DATETIME:
   case DB_TEXT:
   case DB_BINARY:
      break;
   }
   COL_ERROR_STREAM(DBvariantTypeName(DataType) << " is not a numeric "
      "type: cannot convert to integer.", COLerror::PreCondition);
}

double DBvariant::toDouble() const
{
   COL_METHOD(DBvariant::toDouble);
   DBvariantTraceType(DataType);
   switch(DataType) {
   case DB_INTEGER:       return (double)integer32();
   case DB_LARGE_INTEGER: return (double)integer64();
   case DB_DOUBLE:        return (double)float32();
   case DB_LARGE_DOUBLE:  return (double)float64();
   case DB_NULL:
   case DB_DATA_TYPE_NOT_DEFINED:
   case DB_BOOLEAN:
   case DB_STRING:
   case DB_DATETIME:
   case DB_TEXT:
   case DB_BINARY:
      break;
   }
   COL_ERROR_STREAM(DBvariantTypeName(DataType) << " is not a numeric "
      "type: cannot convert to double.", COLerror::PreCondition);
}
