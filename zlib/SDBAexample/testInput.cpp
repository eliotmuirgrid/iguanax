// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testInput
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:01:00 EST
// ---------------------------------------------------------------------------
#include "testInput.h"

#include <UNIT/UNITapp.h>

#include <SDBA/SDBAresubmit.h>
#include <SDBU/SDBUparams.h>

#include <CFG/CFGmap.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>

#include <BLUA/BLUAmanager.h>
COL_LOG_MODULE;


static void SDBATresponder(const COLvar& Response, COLvar* pOutput) {
   COL_FUNCTION(SDBATresponder);
   *pOutput = Response;
   COL_VAR(*pOutput);
}

static COLwebRequest setupWebRequest(){
   COLwebRequest Request;
   Request.User = "admin";
   Request.Address.ConnectionId = -1;
   return Request;
} 

static void testNoComponent(CFGmap* pMap){
   COL_FUNCTION(testNoComponent);
   COL_TRC("test no 'component' parameter");
   COLwebRequest Request = setupWebRequest();
   // Try empty request. With no 'component' param specified, the SDBA logic defaults to 'component' = SDB_SERVICE_COMPONENT_ID. 
   // SDBAresubmitStart will still fail since no 'target' is provided.
   try {
      SDBAresubmitStart(Request, pMap, NULL, NULL);
      UNIT_ASSERT(false); // expect SBDAresubmitStart to throw, so we never reach this
   } catch (const COLerror& Error) {
      UNIT_ASSERT_EQUALS("target parameter is required.", Error.description());
   }
}

static void testBadComponent(CFGmap* pMap){
   COL_FUNCTION(testBadComponent);
   COL_TRC("test bad 'component' parameter");
   COLwebRequest Request = setupWebRequest();
   COLvar Output;

   Request.Data["component"] = "asdfas";
   Request.Data["target"]    = "Destination";
   Request.Address.pInternalResponseCallback = COLnewClosure1(SDBATresponder, &Output);

   SDBUparams Params(Request.Data["component"], Request.Data);
   SDBAresubmitBulkValidate(Request.User, pMap, Request.Data["component"], Request.Data["target"], Params.m_LogFilter, Request.Address);
   UNIT_ASSERT(!Output["success"]);
   UNIT_ASSERT_EQUALS("The specified 'component' doesn't exist.", Output["error"]);
}

static void testBadBounds(CFGmap* pMap){
   COL_FUNCTION(testBadBounds);
   COL_TRC("test bad bounds");
   COLwebRequest Request = setupWebRequest();
   COLvar Output;

   Request.Data["component"] = "Source";
   Request.Data["target"]    = "Destination";
   // Try out the case where start > end but forward = true
   Request.Data["forward"]   = true;
   Request.Data["start"]     = "-100";
   Request.Data["end"]       = "-200";
   Request.Address.pInternalResponseCallback = COLnewClosure1(SDBATresponder, &Output);
   
   SDBUparams Params(Request.Data["component"], Request.Data);
   SDBAresubmitBulkValidate(Request.User, pMap, Request.Data["component"], Request.Data["target"], Params.m_LogFilter, Request.Address);
   UNIT_ASSERT(!Output["success"]);
   UNIT_ASSERT_EQUALS("Bad bounds for start and end time", Output["error"]);

   // Now the opposite case where start < end but forward = false
   Request.Data["forward"]   = false;
   Request.Data["start"]     = "-200";
   Request.Data["end"]       = "-100";
   Request.Address.pInternalResponseCallback = COLnewClosure1(SDBATresponder, &Output);

   Params = SDBUparams(Request.Data["component"], Request.Data);
   SDBAresubmitBulkValidate(Request.User, pMap, Request.Data["component"], Request.Data["target"], Params.m_LogFilter, Request.Address);
   UNIT_ASSERT(!Output["success"]);
   UNIT_ASSERT_EQUALS("Bad bounds for start and end time", Output["error"]);
}

static void testNoTarget(CFGmap* pMap){
   COL_FUNCTION(testNoTarget);
   COL_TRC("test no 'target'");
   COLwebRequest Request = setupWebRequest();
   Request.Data["component"] = "Source";
   // SDBAresubmitStart will fail since no target is provided.
   try {
      SDBAresubmitStart(Request, pMap, NULL, NULL);
      UNIT_ASSERT(false); // expect SBDAresubmitStart to throw, so we never reach this
   } catch (const COLerror& Error) {
      UNIT_ASSERT_EQUALS("target parameter is required.", Error.description());
   }
}

static void testBadTarget(CFGmap* pMap){
   COL_FUNCTION(testBadTarget);
   COL_TRC("test bad 'target'");
   COLwebRequest Request = setupWebRequest();
   COLvar Output;

   Request.Data["component"] = "Source";
   Request.Data["target"] = "qwerewq";
   Request.Address.pInternalResponseCallback = COLnewClosure1(SDBATresponder, &Output);

   SDBUparams Params(Request.Data["component"], Request.Data);
   SDBAresubmitBulkValidate(Request.User, pMap, Request.Data["component"], Request.Data["target"], Params.m_LogFilter, Request.Address);
   UNIT_ASSERT(!Output["success"]);
   UNIT_ASSERT_EQUALS("The specified 'target' doesn't exist", Output["error"]);
}

static void testResubmit() {
   COL_FUNCTION(testResubmit);
   CFGmap Map;
   Map.ConfigMap["Source"];
   Map.ConfigMap["Destination"];
   CFGmapLookupSet(&Map);
   testNoComponent(&Map);
   testBadComponent(&Map);
   testBadBounds(&Map);
   testNoTarget(&Map);
   testBadTarget(&Map);
}

void testInput(UNITapp* pApp){
   COL_FUNCTION(testInput);
   pApp->add("input/validate/resubmit", &testResubmit);
}
