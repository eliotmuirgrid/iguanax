#ifndef __TSM_VARIANT_H__
#define __TSM_VARIANT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMvariant
//
// Description:
//
// This is intended to represent the value part of TSMattribute.  For
// the XML parser the variant value is a string.  TBM uses native representations
// of the data.
//
// Author: Eliot Muir
// Date:   Tuesday, February 13th, 2007 @ 10:46:30 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

enum TSMvalueType
{
   TSM_INPLACE_STRING=0,
   TSM_BOOLEAN = 1,
   TSM_COLSTRING = 2,
   TSM_INT32 = 3
};

class TSMvariant 
{
public:
   TSMvariant();
   // This is non virtual because we are assuming no classes will inherit from this class
   ~TSMvariant();

   // POD so the default operator= and copy constructor is fine.

   TSMvalueType valueType() const;
   void setValueType(TSMvalueType NewValue);

   // By design the setInplaceString methods do not set the value type - this is because we assume
   // that these are only being used in the TXMparser and so we don't want the extra overhead of setting it.
   void setInplaceString(const char* pString);
   void setInplaceStringWithSize(const char* pString, COLuint32 Size);  

   void setBoolean(bool Value);
   void setCOLstring(const COLstring& Value);
   void setInt32(COLint32 Value);

   // These will throw if the type does not match or is not an inplace string
   COLint32 asInt32() const;
   bool asBoolean() const;
   COLstring asString() const;
   operator COLstring() const;

private:
   // I haven't bothered to make a union here since in a typical application there will be no more
   // than a dozen of these objects - so no big point saving memory.
   bool BooleanValue;
   COLstring StringValue;
   COLuint32 Uint32Value;
   COLuint16 Uint16Value;
   COLint32 Int32Value;

   const char* pInplaceString;
   COLuint32 SizeOfInplaceString;

   TSMvalueType ValueType;
};

COLostream& operator<<(COLostream& Stream, const TSMvariant& Var);

#endif // end of defensive include
