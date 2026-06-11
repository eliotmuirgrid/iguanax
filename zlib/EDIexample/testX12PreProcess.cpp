//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testX12PreProcess.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/3/23 1:24 PM
//  ---------------------------------------------------------------------------
#include "testX12PreProcess.h"

#include <EDI/EDIx12.h>

#include <UNIT/UNITapp.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testMissingISA() {
   COL_FUNCTION(testMissingISA);
   COLstring MissingISA = "*14*0073268795005  *020226*1534*U*00401*000000001*0*T*>~";
   COLstring IsaAtEnd   = "*14*0073268795005  *020226*1534*U*00401*000000001*0*T*>~ISA*00*          *00*          *ZZ*SENDERISA      *14*0073268795005  *020226*1534*U*00401*000000001*0*T*>~";
   COLstring ErrorMsg;
   UNIT_ASSERT(EDIx12Preprocess(&MissingISA, &ErrorMsg))
   UNIT_ASSERT(EDIx12Preprocess(&IsaAtEnd, &ErrorMsg))
}

static void testX12Custom() {
   COL_FUNCTION(testX12Custom);
   // SegmentDelimiter = ~, ElementDelimiter = *, SubElementDelimiter = :
   COLstring Message = "ISA*00*          *00*          *ZZ*345529167*ZZ*445483154*160222*1301*U*00401*12345*1*P*:~GS*PO*SENDERGS*007326879*20020226*1534*1*X*004010~ST*850*000000001~BEG*00*SA*A99999-01**19970214~";
   COLstring ErrorMsg;
   EDIx12Preprocess(&Message, &ErrorMsg);
   COL_VAR(Message);
   COLstring Expected = "ISA|00|          |00|          |ZZ|345529167|ZZ|445483154|160222|1301|U|00401|12345|1|P|^\rGS|PO|SENDERGS|007326879|20020226|1534|1|X|004010\rST|850|000000001\rBEG|00|SA|A99999-01||19970214\r";
   COL_TRC_DUMP(Expected, Expected.c_str(), Expected.size());
   COL_TRC_DUMP(Message, Message.c_str(), Message.size());
   UNIT_ASSERT_EQUALS(Expected, Message);
}

static void testX12Simple(){
   COL_FUNCTION(testX12Simple);
   // SegmentDelimiter = ~, ElementDelimiter = *, SubElementDelimiter = :
   COLstring Message = "ISA*00*          *00*          *ZZ*SENDERISA      *14*0073268795005  *020226*1534*U*00401*000000001*0*T*:~";
   COLstring ErrorMsg;
   EDIx12Preprocess(&Message, &ErrorMsg);
   COL_VAR2(Message, ErrorMsg);
   UNIT_ASSERT_EQUALS("ISA|00|          |00|          |ZZ|SENDERISA      |14|0073268795005  |020226|1534|U|00401|000000001|0|T|^\r", Message);
}

void testX12PreProcess(UNITapp* pApp){
   pApp->add("preprocess/x12_simple",  testX12Simple);
   pApp->add("preprocess/x12_custom",  testX12Custom);
   pApp->add("preprocess/missing_isa", testMissingISA);
}