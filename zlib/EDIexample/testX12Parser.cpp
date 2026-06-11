//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testX12Parser.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/8/23 11:08 AM
//  ---------------------------------------------------------------------------
#include "testX12Parser.h"

#include <COL/COLlog.h>
#include <COL/COLhashmap.h>
#include <COL/COLvar.h>
#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIload.h>
#include <EDI/EDIparse.h>
#include <EDI/EDIx12.h>
#include <FIL/FILutils.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void loadCollection(COLstring VmdFile, EDIcollection* pCollection) {
   COL_FUNCTION(loadCollection);
   FILcorrectPathSeparators(VmdFile);
   COLstring Vmd;
   COL_TRC("Loading file");
   if(FILfileExists(VmdFile)) { FILreadFile(VmdFile, &Vmd); }
   else { COL_TRC("File not found"); }
   COLstring Error;
   bool IsLegacy = false;
   COLstring ConversionErrors;
   EDIsmartLoad(Vmd, pCollection, &Error, &ConversionErrors, &IsLegacy);
}

static void testX12MultiMessage() {
   COL_FUNCTION(testX12MultiMessage);
   auto pCollection = new EDIcollection();
   loadCollection("x12/x12_5010_Header_Only_modified.vmd", pCollection);
   COLstring Err;
   COLstring Data("ISA*00*          *AB*          *ZZ*345529167*ZZ*445483154*160222*1301*U*00401*12345*1*P*:~GS*HS*1234567*9876543*160222*1301*81380*X*004010~ST*270*1234~BHT*0022*13*10001234*19990501*1319~HL*1**20*1~NM1*PR*2*ABC COMPANY*****PI*842610001~HL*2*1*21*1~NM1*1P*1*JONES*MARCUS****SV*0202034~HL*3*2*22*0~TRN*1*93175-012547*9877281234~NM1*IL*1*SMITH*ROBERT*B***MI*11122333301~REF*1L*599119~DMG*D8*19430519*M~DTP*472*D8*19990501~EQ*98**FAM~SE*14*1234~GE*1*78072~IEA*1*12345~");
   COLstring Match;
   bool Success = EDIx12Parse(*pCollection, &Data, &Match, &Err);
   UNIT_ASSERT(Success)
   delete pCollection;
}

static void testX12EmptyMessage() {
   COL_FUNCTION(testX12EmptyMessage);
   auto pCollection = new EDIcollection();
   loadCollection("x12/simple_x12.vmd", pCollection);
   COLstring Err;
   COLstring Data("ISA*00*          *AB*          *ZZ*345529167*ZZ*445483154*160222*1301*U*00401*12345*1*P*:~GS*HS*1234567*9876543*160222*1301*81380*X*004010~ST*270*1234~BHT*0022*13*10001234*19990501*1319~HL*1**20*1~NM1*PR*2*ABC COMPANY*****PI*842610001~HL*2*1*21*1~NM1*1P*1*JONES*MARCUS****SV*0202034~HL*3*2*22*0~TRN*1*93175-012547*9877281234~NM1*IL*1*SMITH*ROBERT*B***MI*11122333301~REF*1L*599119~DMG*D8*19430519*M~DTP*472*D8*19990501~EQ*98**FAM~SE*14*1234~GE*1*78072~IEA*1*12345~");
   COLstring Match;
   pCollection->MatchRules.clear();
   bool Success = EDIx12Parse(*pCollection, &Data, &Match, &Err);
   UNIT_ASSERT(Success);
   UNIT_ASSERT_EQUALS("No matching rules were found in the VMD file.\nPlease update the VMD to include matching rules,\nor to parse using this VMD, provide a message name to match.", Err);
   delete pCollection;
}

static void testX12Simple() {
   COL_FUNCTION(testX12Simple);
   auto pCollection = new EDIcollection();
   loadCollection("x12/simple_x12.vmd", pCollection);
   COLstring Err;
   COLstring Data("ISA*00*          *AB*          *ZZ*345529167*ZZ*445483154*160222*1301*U*00401*12345*1*P*:~GS*HS*1234567*9876543*160222*1301*81380*X*004010~ST*270*1234~BHT*0022*13*10001234*19990501*1319~HL*1**20*1~NM1*PR*2*ABC COMPANY*****PI*842610001~HL*2*1*21*1~NM1*1P*1*JONES*MARCUS****SV*0202034~HL*3*2*22*0~TRN*1*93175-012547*9877281234~NM1*IL*1*SMITH*ROBERT*B***MI*11122333301~REF*1L*599119~DMG*D8*19430519*M~DTP*472*D8*19990501~EQ*98**FAM~SE*14*1234~GE*1*78072~IEA*1*12345~");
   COLstring Match;
   bool Success = EDIx12Parse(*pCollection, &Data, &Match, &Err);
   UNIT_ASSERT(Success);
   delete pCollection;
}

static void testX12Main() {
   COL_FUNCTION(testX12Main);
   COLstring Data, ErrorMsg;
   FILreadFile("x12/message_x12.txt", &Data);
   auto pCollection = new EDIcollection();
   loadCollection("x12/simple_x12.vmd", pCollection);
   COLstring Match;
   UNIT_ASSERT(EDIx12Parse(*pCollection, &Data, &Match, &ErrorMsg));
   delete pCollection;
}

static void testX12BadName(){
   COL_FUNCTION(testX12BadName);
   COLstring Data, ErrorMsg;
   FILreadFile("x12/name_message.txt", &Data);
   auto pCollection = new EDIcollection();
   loadCollection("x12/x12_name.vmd", pCollection);
   COLstring BadName  = "NM1";
   COL_TRC("Parse using message name NM1 - a node that exists but isn't a message type node.");
   UNIT_ASSERT(EDIx12Parse(*pCollection, &Data, &BadName, &ErrorMsg));
   UNIT_ASSERT_EQUALS(ErrorMsg, "Message name NM1 does not exist. Try one of these instead: 270B1 271B1 ");
   delete pCollection;
}

void testX12Parser(UNITapp* pApp) {
   pApp->add("parse/x12/simple",          &testX12Simple);
   pApp->add("parse/x12/main",            &testX12Main);
   pApp->add("parse/x12/empty_message",   &testX12EmptyMessage);
   pApp->add("parse/x12/batch", &testX12MultiMessage);
   pApp->add("parse/x12/name",            &testX12BadName);
}