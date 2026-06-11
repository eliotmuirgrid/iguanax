// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testPreprocess.cpp
//
//  Description
//
//  Author: Eliot Muir 
//  Date:   Thu 27 Oct 2022 11:14:37 EDT
// ---------------------------------------------------------------------------
#include "testPreprocess.h"

#include <UNIT/UNITapp.h>
#include <EDI/EDIhl7.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testMissingMsh() {
   COL_FUNCTION(testCustom);
   COLstring MissingMsh = "iNTERFACEWARE\r\n[Lab[Main HIS[St. Micheals[20110213144932[[ADT%A03[";
   COLstring MshAtEnd = "iNTERFACEWARE\r\n[Lab[Main HIS[St. Micheals[20110213144932[[ADT%A03[MSH[%~\\&[";
   COLstring ErrorMsg;
   UNIT_ASSERT(!EDIhl7Preprocess(&MissingMsh, &ErrorMsg))
   UNIT_ASSERT(!EDIhl7Preprocess(&MshAtEnd, &ErrorMsg))
}

static void testCustom() {
   COL_FUNCTION(testCustom);
   // field and subfield custom delimeters -- '[' and '%'
   COLstring Message = "MSH[%~\\&[iNTERFACEWARE\r\n[Lab[Main HIS[St. Micheals[20110213144932[[ADT%A03[";
   COLstring ErrorMsg;
   EDIhl7Preprocess(&Message, &ErrorMsg);
   COLstring Expected = "MSH|||iNTERFACEWARE\r|Lab|Main HIS|St. Micheals|20110213144932||ADT^A03|";
   COL_TRC_DUMP(Expected, Expected.c_str(), Expected.size());
   COL_TRC_DUMP(Message, Message.c_str(), Message.size());
   UNIT_ASSERT_EQUALS(Expected, Message);
}

static void testSimple(){
   COL_FUNCTION(testSimple);
   COLstring Message = "MSH|^~\\&|";
   COLstring ErrorMsg;
   EDIhl7Preprocess(&Message, &ErrorMsg);
   COL_VAR2(Message, ErrorMsg);
   UNIT_ASSERT_EQUALS("MSH|||", Message);
}

void testPreprocess(UNITapp* pApp){
   pApp->add("preprocess/custom",      &testCustom);
   pApp->add("preprocess/simple",      &testSimple);
   pApp->add("preprocess/missing_msh", &testMissingMsh);
}