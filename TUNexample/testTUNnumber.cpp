// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTUNnumber
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Wednesday 14 June 2023 - 08:45AM
// ---------------------------------------------------------------------------
#include "testTUNnumber.h"

#include <UNIT/UNITapp.h>

#include <TUN/TUNnumber.h>
#include <TUN/TUNstring.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testEncodeHelper(COLuint32 Number1){
   COLstring Buffer;
   int Size1;
   Size1 = TUNencodeNumber(Number1, Buffer.get_buffer());
   Buffer.setSize(Size1);
   COL_DUMP("Buffer:", Buffer.get_buffer(), Size1);
   COLuint32 Number2;
   int Size2 = TUNdecodeNumber(&Number2, (const unsigned char*)Buffer.c_str());
   COL_VAR2(Number1, Number2);
   COL_VAR2(Size1, Size2);
   UNIT_ASSERT_EQUALS(Number1, Number2);
   UNIT_ASSERT_EQUALS(Size1, Size2);
}

static void testEncode(){
   COL_FUNCTION(testEncode);
   testEncodeHelper(0x7F);
   testEncodeHelper(0xFF);
   testEncodeHelper(0x1FF);
   testEncodeHelper(0x180);
   
   testEncodeHelper(0xFFFF);
   testEncodeHelper(0xFFFFFF);   
   testEncodeHelper(0xFFFFFFFF);
}

static void testOverFlow(){
   COL_FUNCTION(testOverFlow);
   COLstring Buffer;
   Buffer.setSize(6);
   Buffer[0] = 0xFF;
   Buffer[1] = 0xFF;
   Buffer[2] = 0xFF;
   Buffer[3] = 0xFF;
   Buffer[4] = 0xFF;
   Buffer[5] = 0xFF;
   COLuint32 Number;
   int Size = TUNdecodeNumber(&Number, (const unsigned char*)Buffer.c_str());
   COL_VAR2(Size, Number);
}

static void testStringEncode(){
   COL_FUNCTION(testStringEncode);
   COLstring OutBuffer;
   TUNstringWrite(&OutBuffer, "Fred Smith");
   TUNstringWrite(&OutBuffer, "Mary Goldstein");

   COL_DUMP("Out:", OutBuffer.c_str(), OutBuffer.size());
}

void testTUNnumber(UNITapp* pApp) {
   pApp->add("number/encode",   &testEncode);
   pApp->add("number/overflow", &testOverFlow);
   pApp->add("string/encode",   &testStringEncode);
}
