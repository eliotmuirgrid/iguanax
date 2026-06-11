#ifndef __DB_VARIANT_H__
#define __DB_VARIANT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBvariant
//
// Description:
//
// DBvariant class is a light DB2 specific replacement for the usage
// of TREvariant.  Long term I guess we'll need to decide if DB2 needs
// variant class but this is at least a DB2 specific one that only has
// enough complexity to support the needs to DB2.
//
// Author: Eliot Muir
// Date:   Friday, December 28th, 2007 @ 11:54:09 PM
//---------------------------------------------------------------------------

#include "DBminimumInclude.h"

class COLdateTime;

class DBvariant 
{
public:
   DBvariant();
   DBvariant(COLint32 Integer);
   DBvariant(COLint64 Integer);

   DBvariant(COLfloat32 Float);
   DBvariant(COLfloat64 Float);

   DBvariant(const COLstring& String);
   DBvariant(const COLstring& String, DBdataType Type);  // ALWAYS DEFAULTS TO DB_VARIANT to distinguish it from the DB_STRING

   DBvariant(const char* pString);

   DBvariant(const COLdateTime& DateTime);

   DBvariant(bool Value);

   DBvariant(const DBvariant& Orig); 
   DBvariant& operator=(const DBvariant& Orig); 
   // This is non virtual because we are assuming no classes will inherit from this class
   ~DBvariant();

   DBdataType dataType() const { return DataType; }

   COLstring toString() const;

   // Convert whatever number we have into whatever number you want.
   COLint64 toInteger() const;
   double   toDouble()  const;
 
   //returns true if the two variants are equal
   bool operator==(const DBvariant& Rhs) const;

   const COLdateTime& dateTime() const;
   const COLstring& string() const;
   const COLint32& integer32() const;
   const COLint64& integer64() const;
   const COLfloat32& float32() const;
   const COLfloat64& float64() const;
   const bool& boolean() const;
   const COLstring& binary() const;

   COLdateTime& dateTime();
   COLstring& string();
   COLint32& integer32();
   COLint64& integer64();
   COLfloat32& float32();
   COLfloat64& float64();
   bool& boolean();
   COLstring& binary();

   // Is this null?  "" is null for strings, 0 is null for integers etc.
   bool isNull() const; 

   COLuint32 binarySize() const;
private:
   void cleanUpValue();
   void initType();
   void verifyType(DBdataType DataType) const;
   
   DBdataType DataType;
   union
   {
      COLint32 Integer;           // DB_INTEGER 
      COLint64* pLargeInteger;     // DB_LARGE_INTEGER
      COLfloat32 Float;           // DB_DOUBLE 
      COLfloat64* pLargeFloat;     // DB_LARGE_FLOAT 
      COLdateTime* pDateTime;     // DB_DATETIME
      COLstring* pString;         // DB_TEXT or DB_STRING  or DB_BINARY
      bool Boolean;
   } Value;
};

COLstring DBdataTypeName(DBdataType DataType);

// This is only used for date/time support in the MySQL wrapper currently.
DBvariant DBvariantFromString(const COLstring& String, DBdataType DataType);

const char* DBvariantTypeName(DBdataType DataType);

COLstring DBvariantDebugString(const DBvariant& Var);

#endif // end of defensive include
