//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TBMwriter
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, February 9th, 2007 @ 07:32:49 PM
//
//---------------------------------------------------------------------------
#include "TBMprecomp.h"
#pragma hdrstop

#include "TBMwriter.h"
#include "TBMcode.h"

#include <COL/COLendian.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <TSM/TSMlabel.h>


TBMwriter::TBMwriter(COLsink& iSink)
: Sink(iSink)
{

}

TBMwriter::~TBMwriter(){

}

void TBMwriter::beginObject(const TSMlabel& Label){
   COL_METHOD(TBMwriter::beginObject);
   COL_VAR((int)Label.id());
   Sink.write(&TBMobject, 1);
   Sink.write(&Label.id(), 1);
}

void TBMwriter::addBoolean(const TSMlabel& Label, bool Value){
   COL_METHOD(TBMwriter::addBoolean);
   COL_VAR(Value);
   if (Value){
      Sink.write(&TBMbooleanTrue, 1);
   } else {
      Sink.write(&TBMbooleanFalse, 1); 
   }
   Sink.write(&Label.id(), 1);
}

void TBMwriter::addInt32 (const TSMlabel& Label, COLint32 ValueSigned){
   COL_METHOD(TBMwriter::addInt32);
   COL_VAR2((int)Label.id(), ValueSigned);
   COLuint32 Value = ValueSigned;
   COL_TRC("Adding UINT32=" << hex << Value);
   if (Value < 256)
   {
      Sink.write(&TBMuint32OneByte, 1);
      Sink.write(&Label.id(), 1);
#ifdef WORDS_BIGENDIAN
      Value = COLsystemToLittleEndian(Value);
#endif
      COL_TRC("After Endian Conversion=" << hex << Value);
      Sink.write(&Value, 1);   
   }
   else if (Value <= 0xFFFF)
   {
      Sink.write(&TBMuint32TwoByte, 1);
      Sink.write(&Label.id(), 1);
#ifdef WORDS_BIGENDIAN
      Value = COLsystemToLittleEndian(Value);
#endif
      COL_TRC("After Endian Conversion=" << Value);
      Sink.write(&Value, 2);     
   }
   else
   {
      Sink.write(&TBMuint32FourByte, 1);
      Sink.write(&Label.id(), 1);
#ifdef WORDS_BIGENDIAN
      Value = COLsystemToLittleEndian(Value);
#endif
      COL_TRC("After Endian Conversion=" << Value);
      Sink.write(&Value, 4);  // TODO Endian
   }
}

void TBMwriter::addString (const TSMlabel& Label, const COLstring& Value){
   COL_METHOD(TBMwriter::addString);
   COL_VAR2((int)Label.id(), Value);
   if (Value.size() <= 0xFF){
      Sink.write(&TBMstringShort, 1);
      Sink.write(&Label.id(), 1);
      COLuint8 Size = Value.size();
      Sink.write(&Size, 1);
      Sink.write(Value.c_str(), Value.size());            
   } else if (Value.size() <= 0xFFFF){
      Sink.write(&TBMstringMedium, 1);
      Sink.write(&Label.id(), 1);
      COLuint16 Size = Value.size();
#ifdef WORDS_BIGENDIAN
      Size = COLsystemToLittleEndian(Size);
#endif
      Sink.write(&Size, 2);
      Sink.write(Value.c_str(), Value.size());
   } else{
      Sink.write(&TBMstringLong, 1);
      Sink.write(&Label.id(), 1);
      COLuint32 Size = Value.size();
#ifdef WORDS_BIGENDIAN
      Size = COLsystemToLittleEndian(Size);
#endif
      Sink.write(&Size, 4);
      Sink.write(Value.c_str(), Value.size());
   }
}

void TBMwriter::endPropertyList(const TSMlabel& Label){
   // don't think we need any thing
}

void TBMwriter::endSelfContainedObject(const TSMlabel& Label){
   Sink.write(&TBMendObject,1);
}

void TBMwriter::endObject(const TSMlabel& Label){
   COL_METHOD(TBMwriter::endObject);
   COL_VAR((int)Label.id());
   Sink.write(&TBMendObject,1);
}

void TBMwriter::flush(){
   Sink.flush();
}
