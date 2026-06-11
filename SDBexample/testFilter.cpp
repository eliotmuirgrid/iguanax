// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testFile
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:01:00 EST
// ---------------------------------------------------------------------------
#include "testFilter.h"

#include <COL/COLlog.h>
#include <SDB/SDBfilter.h>
#include <SDB/SDBfilter2.h>
#include <SDB/SDBmessage.h>
#include <SDBU/SDBUparams.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testLog() {
   COL_FUNCTION(testLog);
   SDBfilterInfo Filter = SDBlogFilter();
   UNIT_ASSERT(Filter.Types[SDB_MESSAGE]);
   UNIT_ASSERT(Filter.Types[SDB_INFO]);
   UNIT_ASSERT(Filter.Types[SDB_WARNING]);
   UNIT_ASSERT(Filter.Types[SDB_ERROR]);

   SDBfilterInfo2 Filter2 = SDBlogFilter2();
   UNIT_ASSERT(Filter2.m_Types[SDB_MESSAGE]);
   UNIT_ASSERT(Filter2.m_Types[SDB_INFO]);
   UNIT_ASSERT(Filter2.m_Types[SDB_WARNING]);
   UNIT_ASSERT(Filter2.m_Types[SDB_ERROR]);
}

static void testQueue() {
   COL_FUNCTION(testQueue);
   SDBfilterInfo Filter = SDBqueueFilter();
   UNIT_ASSERT(Filter.Types[SDB_MESSAGE]);
   UNIT_ASSERT(Filter.MessageId.isNull());

   SDBfilterInfo2 Filter2 = SDBqueueFilter2();
   UNIT_ASSERT(Filter2.m_Types[SDB_MESSAGE]);
   UNIT_ASSERT(Filter2.m_Start.isNull());

   SDBmessageId MessageId("dummy");
   Filter = SDBqueueFilter(MessageId);
   UNIT_ASSERT_EQUALS(MessageId, Filter.MessageId);
   UNIT_ASSERT_EQUALS(false, Filter.Inclusive);

   Filter2 = SDBqueueFilter2(MessageId);
   UNIT_ASSERT_EQUALS(MessageId, Filter2.m_Start);
   UNIT_ASSERT_EQUALS(true, Filter2.m_Exclusive);
}

static void testJournal() {
   COL_FUNCTION(testJournal);
   SDBfilterInfo Filter = SDBjournalFilter();
   UNIT_ASSERT(Filter.Types[SDB_POSMAP]);
   UNIT_ASSERT(Filter.Types[SDB_POSITION]);
   UNIT_ASSERT(Filter.Types[SDB_MESSAGE]);
   UNIT_ASSERT(Filter.Types[SDB_INFO]);
   UNIT_ASSERT(Filter.Types[SDB_WARNING]);
   UNIT_ASSERT(Filter.Types[SDB_ERROR]);

   SDBfilterInfo2 Filter2 = SDBjournalFilter2();
   UNIT_ASSERT(Filter2.m_Types[SDB_POSMAP]);
   UNIT_ASSERT(Filter2.m_Types[SDB_POSITION]);
   UNIT_ASSERT(Filter2.m_Types[SDB_MESSAGE]);
   UNIT_ASSERT(Filter2.m_Types[SDB_INFO]);
   UNIT_ASSERT(Filter2.m_Types[SDB_WARNING]);
   UNIT_ASSERT(Filter2.m_Types[SDB_ERROR]);
}

static bool SDBTtypeAllowed(SDBfilterInfo Filter, SDBtype Type) {
   bool       Allowed = false;
   SDBmessage Message("component_id", "boo");
   Message.Type = Type;
   SDBfilterMatch((SDBcontent*)&Message, &Allowed, Filter);
   return Allowed;
}

static bool SDBTtypeAllowed(SDBfilterInfo2 Filter, const SDBtype Type) {
   bool       Allowed = false;
   SDBmessage Message("component_id", "boo");
   Message.Type = Type;
   SDBfilterMatch2((SDBcontent*)&Message, &Allowed, Filter);
   return Allowed;
}

static bool SDBTdataAllowed(SDBfilterInfo Filter, const COLstring& Data) {
   bool       Allowed = false;
   SDBmessage Message("component_id", Data);
   Message.Type = SDB_INFO;
   SDBfilterMatch((SDBcontent*)&Message, &Allowed, Filter);
   return Allowed;
}

static bool SDBTdataAllowed(SDBfilterInfo2 Filter, const COLstring& Data) {
   bool       Allowed = false;
   SDBmessage Message("component_id", Data);
   Message.Type = SDB_INFO;
   SDBfilterMatch2((SDBcontent*)&Message, &Allowed, Filter);
   return Allowed;
}

static bool SDBTidAllowed(SDBfilterInfo Filter, const SDBmessageId& Id) {
   bool       Allowed = false;
   SDBmessage Message(Id.ComponentId, "boo", Id.DateTime, Id.Index);
   Message.Type        = SDB_INFO;
   Message.Id.MessageId.DateTime = Id.DateTime;
   SDBfilterMatch((SDBcontent*)&Message, &Allowed, Filter);
   return Allowed;
}

static bool SDBTidAllowed(SDBfilterInfo2 Filter, const SDBmessageId& Id) {
   bool       Allowed = false;
   SDBmessage Message(Id.ComponentId, "boo", Id.DateTime, Id.Index);
   Message.Type        = SDB_INFO;
   Message.Id.MessageId.DateTime = Id.DateTime;
   SDBfilterMatch2((SDBcontent*)&Message, &Allowed, Filter);
   return Allowed;
}

static void SDBTdisableMostTypes(SDBfilterInfo2* pFilter){
   // Have to disable all message types
   SDBsetFilterType(pFilter, SDB_NULL,     false);
   SDBsetFilterType(pFilter, SDB_MARKER,   false);
   SDBsetFilterType(pFilter, SDB_POSMAP1,  false);
   SDBsetFilterType(pFilter, SDB_POSITION, false);
   SDBsetFilterType(pFilter, SDB_MESSAGE,  false);
   SDBsetFilterType(pFilter, SDB_TRACE,    false);
   SDBsetFilterType(pFilter, SDB_DEBUG,    false);
   SDBsetFilterType(pFilter, SDB_WARNING,  false);
   SDBsetFilterType(pFilter, SDB_ERROR,    false);
   SDBsetFilterType(pFilter, SDB_SUCCESS,  false);
   SDBsetFilterType(pFilter, SDB_POSMAP,   false);
}

static void testFancy() {
   COL_FUNCTION(testFancy);
   SDBfilterInfo Filter;
   Filter.Search = "boo";
   SDBsetFilterType(&Filter, SDB_INFO);

   UNIT_ASSERT_EQUALS(false, SDBTtypeAllowed(Filter, SDB_MARKER));
   UNIT_ASSERT_EQUALS(false, SDBTtypeAllowed(Filter, SDB_POSMAP));
   UNIT_ASSERT_EQUALS(false, SDBTtypeAllowed(Filter, SDB_POSITION));
   UNIT_ASSERT_EQUALS(false, SDBTtypeAllowed(Filter, SDB_WARNING));
   UNIT_ASSERT_EQUALS(false, SDBTtypeAllowed(Filter, SDB_ERROR));
   UNIT_ASSERT_EQUALS(true,  SDBTtypeAllowed(Filter, SDB_INFO));

   UNIT_ASSERT_EQUALS(false, SDBTdataAllowed(Filter, "hello"));
   UNIT_ASSERT_EQUALS(true, SDBTdataAllowed(Filter, "Boo"));
   UNIT_ASSERT_EQUALS(true, SDBTdataAllowed(Filter, "book"));
   UNIT_ASSERT_EQUALS(true, SDBTdataAllowed(Filter, "bamboo"));

   // SDBFilter2 expects a filter setup via SDBUparams, so this part is a bit different.
   COLvar Setup;
   Setup["search"]       = "boo";
   Setup["forward"]      = true; // need this for the IdOne IdTwo tests a bit lower down
   COLstring ComponentId = "component_id";
   SDBUparams Param(ComponentId, Setup);
   // We're only testing the filter so its ok to apply a search immediately
   Param.m_LogFilter.m_Search = "boo";

   // Only allow info message type
   SDBTdisableMostTypes(&Param.m_LogFilter);
   SDBsetFilterType(&Param.m_LogFilter, SDB_INFO);

   UNIT_ASSERT_EQUALS(false, SDBTtypeAllowed(Param.m_LogFilter, SDB_MARKER));
   UNIT_ASSERT_EQUALS(false, SDBTtypeAllowed(Param.m_LogFilter, SDB_POSMAP));
   UNIT_ASSERT_EQUALS(false, SDBTtypeAllowed(Param.m_LogFilter, SDB_POSITION));
   UNIT_ASSERT_EQUALS(false, SDBTtypeAllowed(Param.m_LogFilter, SDB_WARNING));
   UNIT_ASSERT_EQUALS(false, SDBTtypeAllowed(Param.m_LogFilter, SDB_ERROR));
   UNIT_ASSERT_EQUALS(true, SDBTtypeAllowed(Param.m_LogFilter, SDB_INFO));

   UNIT_ASSERT_EQUALS(false, SDBTdataAllowed(Param.m_LogFilter, "hello"));
   UNIT_ASSERT_EQUALS(true, SDBTdataAllowed(Param.m_LogFilter, "Boo"));
   UNIT_ASSERT_EQUALS(true, SDBTdataAllowed(Param.m_LogFilter, "book"));
   UNIT_ASSERT_EQUALS(true, SDBTdataAllowed(Param.m_LogFilter, "bamboo"));

   COLuint64    DateTime    = time(NULL) * 1000;
   SDBmessageId IdOne(ComponentId, DateTime, 42);
   SDBmessageId IdTwo(ComponentId, DateTime, 43);
   SDBfilterAfterId(&Filter, IdOne);
   UNIT_ASSERT_EQUALS(false, SDBTidAllowed(Filter, IdOne));
   UNIT_ASSERT_EQUALS(true, SDBTidAllowed(Filter, IdTwo));
   SDBfilterAfterId(&Filter, IdOne, true);
   UNIT_ASSERT_EQUALS(true, SDBTidAllowed(Filter, IdOne));
   UNIT_ASSERT_EQUALS(true, SDBTidAllowed(Filter, IdTwo));

   SDBfilterAfterId2(&Param.m_LogFilter, IdOne, IdTwo);
   UNIT_ASSERT_EQUALS(false, SDBTidAllowed(Param.m_LogFilter, IdOne));
   UNIT_ASSERT_EQUALS(true,  SDBTidAllowed(Param.m_LogFilter, IdTwo));
   SDBfilterAfterId2(&Param.m_LogFilter, IdOne, IdTwo, false);
   UNIT_ASSERT_EQUALS(true, SDBTidAllowed(Param.m_LogFilter, IdOne));
   UNIT_ASSERT_EQUALS(true, SDBTidAllowed(Param.m_LogFilter, IdTwo));
}

void testFilter(UNITapp* pApp) {
   COL_FUNCTION(testFilter);
   pApp->add("filter/log", &testLog);
   pApp->add("filter/queue", &testQueue);
   pApp->add("filter/journal", &testJournal);
   pApp->add("filter/fancy", &testFancy);
}
