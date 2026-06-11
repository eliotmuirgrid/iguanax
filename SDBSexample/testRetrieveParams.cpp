//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testRetrieve.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-27, 12:15p.m.
//  ---------------------------------------------------------------------------
#include "testRetrieveParams.h"

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <SDBS/SDBSretrieveState.h>
#include <SDBU/SDBUparams.h>
#include <SDBU/SDBUutils.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static COLvar createTestRequestData(const COLstring& StartTime, const COLstring& StartId, const COLstring& EndTime,
                                    const COLstring& EndId, const bool Forward) {
   COLvar RequestData;
   RequestData["start"]    = StartTime;
   RequestData["start_id"] = StartId;
   RequestData["end"]      = EndTime;
   RequestData["end_id"]   = EndId;
   RequestData["forward"]  = Forward;
   RequestData["limit"]    = 20;
   return RequestData;
}

static void testParams() {
   COL_FUNCTION(testParams);
   // Forward search with Start > End (bad bounds)
   {
      COLvar             BadForwardData = createTestRequestData("1000", "1", "500", "1", true);
      SDBUparams Params("TestComponent", BadForwardData);
      bool               BadBounds = SDBUbadBounds(Params.m_LogFilter.m_Start, Params.m_LogFilter.m_End, Params.m_LogFilter.m_Forward);
      UNIT_ASSERT_MESSAGE(BadBounds, "Forward search with Start > End should have bad bounds");
   }

   // Forward search with Start < End (good bounds)
   {
      COLvar             GoodForwardData = createTestRequestData("500", "1", "1000", "1", true);
      SDBUparams Params("TestComponent", GoodForwardData);
      bool               BadBounds = SDBUbadBounds(Params.m_LogFilter.m_Start, Params.m_LogFilter.m_End, Params.m_LogFilter.m_Forward);
      UNIT_ASSERT_MESSAGE(!BadBounds, "Forward search with Start < End should have good bounds");
   }

   // Backward search with End > Start (bad bounds)
   {
      COLvar             BadBackwardData = createTestRequestData("1000", "1", "500", "1", false);
      SDBUparams Params("TestComponent", BadBackwardData);
      bool               BadBounds = SDBUbadBounds(Params.m_LogFilter.m_Start, Params.m_LogFilter.m_End, Params.m_LogFilter.m_Forward);
      UNIT_ASSERT_MESSAGE(!BadBounds, "Backward search with End > Start should not have bad bounds");
   }

   // Backward search with End < Start (good bounds)
   {
      COLvar             GoodBackwardData = createTestRequestData("500", "1", "1000", "1", false);
      SDBUparams Params("TestComponent", GoodBackwardData);
      bool               BadBounds = SDBUbadBounds(Params.m_LogFilter.m_Start, Params.m_LogFilter.m_End, Params.m_LogFilter.m_Forward);
      UNIT_ASSERT_MESSAGE(BadBounds, "Backward search with End < Start should have bad bounds");
   }

   // Equal timestamps, different message IDs - Forward
   {
      COLvar             EqualTimeForwardData = createTestRequestData("1000", "5", "1000", "3", true);
      SDBUparams Params("TestComponent", EqualTimeForwardData);
      bool               BadBounds = SDBUbadBounds(Params.m_LogFilter.m_Start, Params.m_LogFilter.m_End, Params.m_LogFilter.m_Forward);
      UNIT_ASSERT_MESSAGE(BadBounds,
                          "Forward search with equal times but Start Index > End Index should have bad bounds");
   }

   // Equal timestamps, different message IDs - Backward
   {
      COLvar             EqualTimeBackwardData = createTestRequestData("1000", "3", "1000", "5", false);
      SDBUparams Params("TestComponent", EqualTimeBackwardData);
      bool               BadBounds = SDBUbadBounds(Params.m_LogFilter.m_Start, Params.m_LogFilter.m_End, Params.m_LogFilter.m_Forward);
      UNIT_ASSERT_MESSAGE(
          BadBounds, "Backward search with equal times but End Index > Start Index should have bad bounds");
   }

   // Identical Start and End (boundary case)
   {
      COLvar             IdenticalData = createTestRequestData("1000", "5", "1000", "5", true);
      SDBUparams Params("TestComponent", IdenticalData);
      bool               BadBounds = SDBUbadBounds(Params.m_LogFilter.m_Start, Params.m_LogFilter.m_End, Params.m_LogFilter.m_Forward);
      UNIT_ASSERT_MESSAGE(!BadBounds, "Identical Start and End should not have bad bounds");
   }
}

static void testParamsWithZeroDefaults() {
   COL_FUNCTION(testParamsWithZeroDefaults);
   // Forward search with zero start defaults to COL_UINT_64_MAX for end
   {
      COLvar             ForwardZeroData = createTestRequestData("0", "0", "0", "0", true);
      SDBUparams Params("TestComponent", ForwardZeroData);
      UNIT_ASSERT_EQUALS(COL_UINT_64_MAX, Params.m_LogFilter.m_End.DateTime);
      UNIT_ASSERT_EQUALS(COL_UINT_64_MAX, Params.m_LogFilter.m_End.Index);
   }

   // Backward search with zero start defaults to COL_UINT_64_MAX for start
   {
      COLvar             BackwardZeroData = createTestRequestData("0", "0", "0", "0", false);
      SDBUparams Params("TestComponent", BackwardZeroData);
      UNIT_ASSERT_EQUALS(COL_UINT_64_MAX, Params.m_LogFilter.m_Start.DateTime);
      UNIT_ASSERT_EQUALS(COL_UINT_64_MAX, Params.m_LogFilter.m_Start.Index);
   }

   // Forward with partial zero values
   {
      COLvar             PartialZeroData = createTestRequestData("1000", "0", "0", "5", true);
      SDBUparams Params("TestComponent", PartialZeroData);
      UNIT_ASSERT_EQUALS(1000, Params.m_LogFilter.m_Start.DateTime);
      UNIT_ASSERT_EQUALS(0, Params.m_LogFilter.m_Start.Index);
      UNIT_ASSERT_EQUALS(COL_UINT_64_MAX, Params.m_LogFilter.m_End.DateTime);
      UNIT_ASSERT_EQUALS(COL_UINT_64_MAX, Params.m_LogFilter.m_End.Index); // SDBmessageId constructor only sets Id if time != 0
   }
}

static void testParamsEdgeCases() {
   COL_FUNCTION(testParamsEdgeCases);
   // Max values
   {
      COLstring          MaxValue = COLuint64ToString(COL_UINT_64_MAX);
      COLvar             MaxData  = createTestRequestData(MaxValue, MaxValue, MaxValue, MaxValue, true);
      SDBUparams Params("TestComponent", MaxData);
      bool               BadBounds = SDBUbadBounds(Params.m_LogFilter.m_Start, Params.m_LogFilter.m_End, Params.m_LogFilter.m_Forward);
      UNIT_ASSERT_MESSAGE(!BadBounds, "Maximum values should not have bad bounds when equal");
   }

   // One value at max, other at zero - Forward
   {
      COLstring          MaxValue     = COLuint64ToString(COL_UINT_64_MAX);
      COLvar             MaxStartData = createTestRequestData(MaxValue, MaxValue, "1", "1", true);
      SDBUparams Params("TestComponent", MaxStartData);
      bool               BadBounds = SDBUbadBounds(Params.m_LogFilter.m_Start, Params.m_LogFilter.m_End, Params.m_LogFilter.m_Forward);
      UNIT_ASSERT_MESSAGE(BadBounds, "Forward search with max start and small end should have bad bounds");
   }

   // One value at max, other at zero - Backward
   {
      COLstring          MaxValue   = COLuint64ToString(COL_UINT_64_MAX);
      COLvar             MaxEndData = createTestRequestData("1", "1", MaxValue, MaxValue, false);
      SDBUparams Params("TestComponent", MaxEndData);
      bool               BadBounds = SDBUbadBounds(Params.m_LogFilter.m_Start, Params.m_LogFilter.m_End, Params.m_LogFilter.m_Forward);
      UNIT_ASSERT_MESSAGE(BadBounds, "Backward search with small start and max end should have bad bounds");
   }

   // Component ID preservation in bounds checking
   {
      COLvar             TestData = createTestRequestData("1000", "1", "500", "1", true);
      SDBUparams Params("DifferentComponent", TestData);
      UNIT_ASSERT_EQUALS("DifferentComponent", Params.m_LogFilter.m_Start.ComponentId);
      UNIT_ASSERT_EQUALS("DifferentComponent", Params.m_LogFilter.m_End.ComponentId);
      UNIT_ASSERT_EQUALS("DifferentComponent", Params.m_Component);
   }
}

void testRetrieveParams(UNITapp* pApp) {
   COL_FUNCTION(testRetrieveParams);
   pApp->add("params", &testParams);
   pApp->add("params/zero_defaults", &testParamsWithZeroDefaults);
   pApp->add("params/edge_cases", &testParamsEdgeCases);
}
