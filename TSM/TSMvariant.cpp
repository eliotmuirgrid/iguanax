//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSMvariant
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, February 13th, 2007 @ 10:46:30 AM
//
//---------------------------------------------------------------------------
#include "TSMprecomp.h"
#pragma hdrstop

#include "TSMvariant.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <string.h>

TSMvariant::TSMvariant()
: ValueType(TSM_INPLACE_STRING),
  pInplaceString(0),
  SizeOfInplaceString(0)
{  
}

TSMvariant::~TSMvariant()
{
}

TSMvalueType TSMvariant::valueType() const
{
   return ValueType;
}

void TSMvariant::setValueType(TSMvalueType NewValue)
{
   ValueType = NewValue;
}

void TSMvariant::setInplaceString(const char* pString)
{
   COLuint32 Size = strlen(pString);
   setInplaceStringWithSize(pString, Size);
}

void TSMvariant::setInplaceStringWithSize(const char* pString, COLuint32 Size)
{
   pInplaceString = pString;
   SizeOfInplaceString = Size;
} 

void TSMvariant::setBoolean(bool Value)
{
   BooleanValue = Value;
   ValueType = TSM_BOOLEAN;
}

void TSMvariant::setCOLstring(const COLstring& Value)
{
   StringValue = Value;
   ValueType = TSM_COLSTRING;
}

void TSMvariant::setInt32(COLint32 Value)
{
   Int32Value = Value;
   ValueType = TSM_INT32;
}

COLint32 TSMvariant::asInt32() const{
   COL_METHOD(TSMvariant::asInt32);
   COL_VAR(ValueType);
   switch(ValueType)
   {
   case TSM_INPLACE_STRING:
      // unfortunately atoi requires a NULL terminated C string
      {
         if(pInplaceString == NULL)
         {
            return 0;
         }
         COLstring Value(pInplaceString, SizeOfInplaceString);   // TODO - could be more efficient.
         return atoi(Value.c_str());            
      }
   case TSM_INT32:
      return Int32Value;
   case TSM_BOOLEAN:
   case TSM_COLSTRING:
   default:
      throw COLerror("Wrong type", COLerror::PreCondition);      
   }
}

bool TSMvariant::asBoolean() const
{
   switch(ValueType)
   {
   case TSM_INPLACE_STRING:
      return (SizeOfInplaceString == 4 && strncmp("true", pInplaceString, 4) == 0);            
   case TSM_BOOLEAN:
      return BooleanValue;      
   case TSM_COLSTRING:
   default:
      throw COLerror("Wrong type", COLerror::PreCondition);            
   }
}

COLstring TSMvariant::asString() const
{
   return *this;
}

TSMvariant::operator COLstring() const
{
   switch(ValueType)
   {
   case TSM_INPLACE_STRING:    
      if(pInplaceString == NULL)
      {
         return "";
      }
      return COLstring(pInplaceString, SizeOfInplaceString);      
   case TSM_COLSTRING:
      return StringValue;
   case TSM_BOOLEAN:
   default:
      throw COLerror("Wrong type", COLerror::PreCondition);            
   }
}
COLostream& operator<<(COLostream& Stream, const TSMvariant& Var){
   switch(Var.valueType()){
   case TSM_COLSTRING:
      Stream << '"' << Var.asString() << '"';
      break;
   case TSM_INT32:
      Stream <<  Var.asInt32();
      break;
   default:
      Stream << "Edit TSMvariant.cpp for more info";
   }
   return Stream;
}
