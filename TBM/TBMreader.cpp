//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TBMreader
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, February 9th, 2007 @ 11:22:14 PM
//
//---------------------------------------------------------------------------
#include "TBMprecomp.h"
#pragma hdrstop

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include "TBMreader.h"
#include "TBMcode.h"

#include <TSM/TSMhandlerStack.h>
#include <TSM/TSMattributeList.h>
#include <TSM/TSMattribute.h>
#include <TSM/TSMlabel.h>

class TBMreaderPrivate
{
public:
   TBMreaderPrivate() 
   : ReadingObjectAttributes(false) {}
   TBMresult parse(const char* pBuffer, COLuint32 SizeOfBuffer, COLuint32& Position,COLuint32& AmountNeeded);

   TSMhandlerStack HandlerStack;
   TSMattributeList AttributeList;
   TSMlabel ObjectId;

   bool ReadingObjectAttributes;
};

TBMreader::TBMreader()
{
   pMember = new TBMreaderPrivate;  
}

TBMreader::~TBMreader()
{
   delete pMember;
}

inline COLuint8 TBMgetUint8(const char* pBuffer)
{
   return *((COLuint8*)pBuffer);
}

// This gives us a single place to deal with fussy platforms that don't like casting
// and endian issues.
inline COLuint16 TBMgetUint16(const char* pBuffer)
{
#ifdef WORDS_BIGENDIAN
   return (COLuint8)pBuffer[0] + ((COLuint8)pBuffer[1] << 8);
#else
   return *((COLuint16*)pBuffer);
#endif
}

inline COLuint32 TBMgetUint32(const char* pBuffer)
{
#ifdef WORDS_BIGENDIAN
   return (COLuint8)pBuffer[0] + ((COLuint8)pBuffer[1] << 8) + ((COLuint8)pBuffer[2] << 16) + ((COLuint8)pBuffer[3] << 24); 
#else
   return *((COLuint32*)pBuffer);
#endif
}

TBMresult TBMreaderPrivate::parse(const char* pBuffer, COLuint32 SizeOfBuffer, COLuint32& Position, COLuint32& AmountNeeded){
   COL_METHOD(TBMreaderPrivate::parse);
   COL_VAR3(SizeOfBuffer, Position, AmountNeeded);
   while (Position < SizeOfBuffer){
      COL_VAR3(Position, (int)pBuffer[Position], ReadingObjectAttributes);
      switch(pBuffer[Position]){
      case TBMobject:{
            COL_TRC("TBMobject");
            if ((Position + 2) > SizeOfBuffer){
               AmountNeeded = 2;
               return TBM_INCOMPLETE_TOKEN;
            }
            if (ReadingObjectAttributes){
               COL_TRC("Reading attributes, call Handler");
               (HandlerStack.currentHandler())(HandlerStack, ObjectId, AttributeList);
               AttributeList.clear();
               HandlerStack.push();
            }
            ObjectId.setId(TBMgetUint8(pBuffer + Position + 1));
            Position+=2;
            ReadingObjectAttributes = true;
            COL_TRC("Object ID=" << (COLuint32)ObjectId.id() << indent);
         }
         break;
      case TBMendObject:{
            COL_TRC("TBMendObject");
            Position++;
            if (ReadingObjectAttributes)
            {
               (HandlerStack.currentHandler())(HandlerStack, ObjectId, AttributeList);
               AttributeList.clear();
               HandlerStack.push();
               ReadingObjectAttributes = false;
            }
            COL_TRC(unindent << "END");
            if (HandlerStack.pop() == 0)
            {
               AmountNeeded = 0;
               return TBM_COMPLETE;
            }    
         }
         break;
      case TBMbooleanFalse:
         {
            COL_TRC("TBMbooleanFalse");
            if ((Position+2) > SizeOfBuffer)
            {  
               AmountNeeded = 2;
               return TBM_INCOMPLETE_TOKEN;
            }
            COLuint8 Id = TBMgetUint8(pBuffer+Position+1);
            TSMattribute& NewAttribute = AttributeList.addAttribute();
            NewAttribute.name().setId(Id);
            NewAttribute.value().setBoolean(false);
            Position+=2;
         }
         break;
      case TBMbooleanTrue:
         {
            COL_TRC("TBMbooleanTrue");
            if ((Position+2) > SizeOfBuffer)
            {  
               AmountNeeded = 2;
               return TBM_INCOMPLETE_TOKEN;
            }
            COLuint8 Id = TBMgetUint8(pBuffer+Position+1);
            TSMattribute& NewAttribute = AttributeList.addAttribute();
            NewAttribute.name().setId(Id);
            NewAttribute.value().setBoolean(true);
            Position+=2;
         }
         break;
      case TBMuint16OneByte:
         {
            COL_TRC("TBMuint16OneByte");
            if ((Position+3) > SizeOfBuffer)
            {
               AmountNeeded = 3;
               return TBM_INCOMPLETE_TOKEN;
            }
            COLuint8 Id = TBMgetUint8(pBuffer+Position+1);
            COLuint16 Value = (COLuint8)*(pBuffer + Position+2);
            TSMattribute& NewAttribute = AttributeList.addAttribute();
            NewAttribute.name().setId(Id);
            NewAttribute.value().setInt32(Value); 
            Position+=3;
         }
         break;
      case TBMuint16TwoByte:
         {
            COL_TRC("TBMuint16TwoByte");
            TBM_LOG("Uint16");
            if ((Position+4) > SizeOfBuffer)
            {
               AmountNeeded = 4;
               return TBM_INCOMPLETE_TOKEN;
            }
            COLuint8 Id = TBMgetUint8(pBuffer+Position+1);
            COLuint16 Value = TBMgetUint16(pBuffer + Position+2);
            TSMattribute& NewAttribute = AttributeList.addAttribute();
            NewAttribute.name().setId(Id);
            NewAttribute.value().setInt32(Value); 
            Position+=4;
         }
         break;
      case TBMuint32OneByte:
         {
            COL_TRC("TBMuint32OneByte");
            if ((Position+3) > SizeOfBuffer)
            {
               AmountNeeded = 3;
               return TBM_INCOMPLETE_TOKEN;
            }
            COLuint8 Id = TBMgetUint8(pBuffer+Position+1);
            COLuint32 Value = (COLuint8)*(pBuffer + Position+2);
            TBM_LOG("Uint32[" << (COLuint32)Id << "] = " << Value);
            TSMattribute& NewAttribute = AttributeList.addAttribute();
            NewAttribute.name().setId(Id);
            NewAttribute.value().setInt32(Value);
            Position+=3;
         }
         break;
      case TBMuint32TwoByte:
         {
            COL_TRC("TBMuint32TwoByte");
            if ((Position+4) > SizeOfBuffer)
            {
               AmountNeeded = 4;               
               return TBM_INCOMPLETE_TOKEN;
            }
            COLuint8 Id = TBMgetUint8(pBuffer+Position+1);
            COLuint32 Value = TBMgetUint16(pBuffer + Position+2);
            COL_TRC("Uint32[" << (COLuint32)Id << "] = " << Value);
            TSMattribute& NewAttribute = AttributeList.addAttribute();
            NewAttribute.name().setId(Id);
            NewAttribute.value().setInt32(Value);
            Position+=4;
         }
         break;
      case TBMuint32FourByte:
         {
            COL_TRC("TBMuint32FourByte");
            if ((Position+6) > SizeOfBuffer)
            {
               AmountNeeded = 6;               
               return TBM_INCOMPLETE_TOKEN;
            }
            COLuint8 Id = TBMgetUint8(pBuffer+Position+1);
            COLuint32 Value = TBMgetUint32(pBuffer + Position+2);
            TBM_LOG("Uint32[" << (COLuint32)Id << "] = " << Value);            
            TSMattribute& NewAttribute = AttributeList.addAttribute();
            NewAttribute.name().setId(Id);
            NewAttribute.value().setInt32(Value);
            Position+=6;
         }
         break;
      case TBMstringShort:
         {
            COL_TRC("TBMstringShort");
            if ((Position + 3) > SizeOfBuffer)
            {
               AmountNeeded = 3;               
               return TBM_NEED_STRING_SIZE;
            }
            COLuint8 SizeOfString = TBMgetUint8(pBuffer+Position+2);
            if ((Position + 3 + SizeOfString) > SizeOfBuffer)
            {
               AmountNeeded = 3 + SizeOfString;                              
               return TBM_INCOMPLETE_TOKEN;
            }
            COLuint8 Id = TBMgetUint8(pBuffer+Position+1); 
            TSMattribute& NewStringAttribute = AttributeList.addAttribute();
            NewStringAttribute.name().setId(Id);
            COLstring Value(pBuffer+Position+3, SizeOfString); 
            NewStringAttribute.value().setCOLstring(Value);
            COL_TRC("String[" << (COLuint32)Id << "] = " << Value);
            
            Position += SizeOfString + 3;
         }
         break;
      case TBMstringMedium:
         {
            COL_TRC("TBMstringMedium");
            if ((Position + 4) > SizeOfBuffer)
            {
               AmountNeeded = 4;               
               return TBM_NEED_STRING_SIZE;
            }
            COLuint16 SizeOfString = TBMgetUint16(pBuffer+Position+2);
            if ((Position + 4 + SizeOfString) > SizeOfBuffer)
            {
               AmountNeeded = 4 + SizeOfString;         
               return TBM_INCOMPLETE_TOKEN;
            }
            COLuint8 Id = TBMgetUint8(pBuffer+Position+1); 
            TSMattribute& NewStringAttribute = AttributeList.addAttribute();
            NewStringAttribute.name().setId(Id);
            COLstring Value(pBuffer+Position+4, SizeOfString); 
            NewStringAttribute.value().setCOLstring(Value);
            Position += SizeOfString + 4;
         }
         break;
      case TBMstringLong:
         {
            COL_TRC("TBMstringLong");
            if ((Position + 6) > SizeOfBuffer)
            {
               AmountNeeded = 6;
               return TBM_NEED_STRING_SIZE;
            }
            COLuint32 SizeOfString = TBMgetUint32(pBuffer+Position+2);
            if ((Position + 6 + SizeOfString) > SizeOfBuffer)
            {
               AmountNeeded = 6 + SizeOfString;
               return TBM_INCOMPLETE_TOKEN;
            }
            COLuint8 Id = TBMgetUint8(pBuffer+Position+1); 
            TSMattribute& NewStringAttribute = AttributeList.addAttribute();
            NewStringAttribute.name().setId(Id);
            COLstring Value(pBuffer+Position+6, SizeOfString); 
            NewStringAttribute.value().setCOLstring(Value);
            Position += SizeOfString + 6;
         }
         break;
      default:
         COL_TRC("TBM_UNKNOWN_BINARY_TOKEN");
         return TBM_UNKNOWN_BINARY_TOKEN;

      }
   }
   AmountNeeded = 0;
   return TBM_INCOMPLETE;
}


TBMresult TBMreader::parse(const char* pBuffer, COLuint32 SizeOfBuffer, COLuint32& Position,COLuint32& AmountNeeded)
{
   TBMASSERT(SizeOfBuffer > 0);
#ifndef TBM_ASSERTS_ON
   return pMember->parse(pBuffer, SizeOfBuffer, Position, AmountNeeded);
#else
   TBMresult Result = pMember->parse(pBuffer, SizeOfBuffer, Position, AmountNeeded);
   switch(Result)
   {
   case TBM_INCOMPLETE:
      TBMASSERT(AmountNeeded == 0);
      TBMASSERT(Position == SizeOfBuffer);
      break;
   case TBM_INCOMPLETE_TOKEN:
      TBMASSERT(Position < SizeOfBuffer)
      TBMASSERT(AmountNeeded > 0);
      break;
   case TBM_NEED_STRING_SIZE:
      TBMASSERT(AmountNeeded >= 3 && AmountNeeded <= 6);
      TBMASSERT(Position < SizeOfBuffer);
      break;
   case TBM_COMPLETE:
      TBMASSERT(AmountNeeded == 0);
      break;
   default:
      TBMASSERT(false);
      break;
   }
   return Result;

#endif
}

TSMhandlerStack& TBMreader::handlerStack()
{
   return pMember->HandlerStack;
}
