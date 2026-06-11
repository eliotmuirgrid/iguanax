// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testRandom
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Wednesday 09 October 2024 - 03:10PM
// ---------------------------------------------------------------------------
#include "testRandom.h"

#include <UNIT/UNITapp.h>

#include <EDI/EDIgrammar.h>
#include <EDI/EDIload.h>
#include <EDI/EDIcopy.h>
#include <EDI/EDIrandom.h>
#include <EDI/EDIrandomPopulate.h>
#include <EDI/EDIprune.h>
#include <EDI/EDIdata.h>

#include <COL/COLref.h>
#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring EDIrandomMessage(const EDIcollection& Grammar, const COLstring& MsgName){
   COL_FUNCTION(EDIrandomMessage);
   COLref<EDIdata> pMessage = new EDIdata(Grammar, MsgName);
   EDIrandomPopulate(pMessage.get());
   COLstring Result;
   pMessage->flatwire(&Result);
   Result.replace("\r", "\n");
   Result = "\n" + Result;
   return Result;
}

static void testRandomSimple(){
   COL_FUNCTION(testRandomSimple);
   EDIcollection Lib, Target;
   COLstring Error;
   bool Success = EDIloadWeb("../Iguana/app/web/HL7/HL7_2_7.js", &Lib, &Error);
   EDIcopy(Lib, &Target, "PID");
   EDIcopy(Lib, &Target, "PV1");
   EDIcopy(Lib, &Target, "IN1");
   EDIcopy(Lib, &Target, "IN2");
   EDIcopy(Lib, &Target, "IN3");

   COLmath::resetRandomGenerator();
   
   EDIsegmentAdd(&Target, "ZED"); // Who's Zed? Zed's dead...
   EDIsegmentAdd(&Target, "ZID"); 
   EDIsegmentGroupAdd(&Target, "ZGRP");
   EDIsegmentGroupAdd(&Target, "ZGRP2");
   EDIsegmentGroupAdd(&Target, "ZGRP3");
   EDInode* pNode = EDIaddRandomNode(&Target, "ZDTA04", "Message", EDI_MESSAGE, EDI_SEGMENT, EDI_SEGMENT_GROUP, 9, "Child");
   EDIcopy(Lib, &Target, "MSH");
   pNode->Children[0].pNode = Target.Nodes["MSH"];
   pNode->Children[0].Required = true;
   EDIremoveRepeatedNodes(&Target, pNode);
   EDIprune(&Target);
   EDIcopy(Lib, &Target, "MDMT10");
   COL_VAR(Target);
   for (int i=0; i < 4; i++){
      COLstring Result = EDIrandomMessage(Target, "MDMT10");
      COL_VAR(Result);
   }
}

void testRandom(UNITapp* pApp) {
   pApp->add("random/simple", &testRandomSimple);
}
