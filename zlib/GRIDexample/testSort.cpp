//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testSort.cpp
//
//  Description: Unit tests for GRIDsort functionality
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLthreadPool.h>
#include <GRID/GRIDnode.h>
#include <GRID/GRIDsort.h>
#include <SCK/SCKloop.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static GRIDconfig GRIDcreateTestConfig(const COLstring& NodeId) {
   COL_FUNCTION(GRIDcreateTestConfig);
   GRIDconfig Config;
   Config.m_NodeInfo.m_IguanaId = NodeId;
   Config.m_NodeInfo.m_Enabled  = true;
   return Config;
}

static GRIDpeerInfo GRIDcreateTestPeerInfo(const COLstring& Name, const COLstring& Host, const int Port) {
   COL_FUNCTION(GRIDcreateTestPeerInfo);
   GRIDpeerInfo Info;
   Info.InstanceName = Name;
   Info.Host         = Host;
   Info.Port         = Port;
   Info.Fingerprint  = "test_fp_" + Name;
   Info.ReadAccess   = GRIDreadAccessLevel::None;
   Info.pConnection  = nullptr;
   Info.LastError.clear();
   return Info;
}

static GRIDpeerInfo GRIDcreateTestPeerInfoError(const COLstring& Name, const COLstring& Host, const int Port,
                                                const COLstring& ErrorMsg) {
   COL_FUNCTION(GRIDcreateTestPeerInfoError);
   GRIDpeerInfo Info;
   Info.InstanceName = Name;
   Info.Host         = Host;
   Info.Port         = Port;
   Info.Fingerprint  = "test_fp_" + Name;
   Info.ReadAccess   = GRIDreadAccessLevel::None;
   Info.pConnection  = nullptr;
   Info.LastError    = ErrorMsg;
   return Info;
}

static GRIDpeerInfo GRIDcreateTestPeerInfoWithMetrics(const COLstring& Name, const COLstring& Host, const int Port,
                                                       const int CompsWithErr, const int MaxQueue, const int Throughput) {
   COL_FUNCTION(GRIDcreateTestPeerInfoWithMetrics);
   GRIDpeerInfo Info;
   Info.InstanceName = Name;
   Info.Host         = Host;
   Info.Port         = Port;
   Info.Fingerprint  = "test_fp_" + Name;
   Info.ReadAccess   = GRIDreadAccessLevel::None;
   Info.pConnection  = nullptr;
   Info.LastError.clear();
   Info.Metrics.CompsWithErr  = CompsWithErr;
   Info.Metrics.MaxQueueCount = MaxQueue;
   Info.Metrics.Throughput    = Throughput;
   return Info;
}

// Helper to directly add a peer to node's peer map (test only since bypasses WebSocket creation)
static void GRIDaddTestPeer(GRIDnode* pNode, const COLstring& PeerId, const GRIDpeerInfo& Info) {
   COL_FUNCTION(GRIDaddTestPeer);
   auto& Peers = const_cast<COLhashmap<COLstring, GRIDpeerInfo>&>(pNode->getPeers());
   Peers[PeerId] = Info;
}

static COLvar GRIDcreateTestPeerEntry(const COLstring& Id, const COLstring& Name) {
   COL_FUNCTION(GRIDcreateTestPeerEntry);
   COLvar Peer;
   Peer["id"]   = Id;
   Peer["name"] = Name;
   return Peer;
}

static COLvar GRIDcreateTestPeerEntryWithMetrics(const COLstring& Id, const COLstring& Name,
                                                  const int CompsWithErr, const int MaxQueue, const int Throughput) {
   COL_FUNCTION(GRIDcreateTestPeerEntryWithMetrics);
   COLvar Peer;
   Peer["id"]               = Id;
   Peer["name"]             = Name;
   Peer["errors"]   = CompsWithErr;
   Peer["queue"]  = MaxQueue;
   Peer["throughput"]       = Throughput;
   return Peer;
}

static void testSortNameAscending() {
   COL_FUNCTION(testSortNameAscending);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDaddTestPeer(&Node, "peer_c", GRIDcreateTestPeerInfo("Charlie", "host3", 3000));
   GRIDaddTestPeer(&Node, "peer_a", GRIDcreateTestPeerInfo("Alpha", "host1", 1000));
   GRIDaddTestPeer(&Node, "peer_b", GRIDcreateTestPeerInfo("Bravo", "host2", 2000));

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntry("peer_c", "Charlie"));
   Response.push_back(GRIDcreateTestPeerEntry("peer_a", "Alpha"));
   Response.push_back(GRIDcreateTestPeerEntry("peer_b", "Bravo"));

   GRIDsortPeers(&Response, &Node, "name", true);

   UNIT_ASSERT(Response[0]["name"].asString() == "Alpha");
   UNIT_ASSERT(Response[1]["name"].asString() == "Bravo");
   UNIT_ASSERT(Response[2]["name"].asString() == "Charlie");
}

static void testSortNameDescending() {
   COL_FUNCTION(testSortNameDescending);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDaddTestPeer(&Node, "peer_a", GRIDcreateTestPeerInfo("Alpha", "host1", 1000));
   GRIDaddTestPeer(&Node, "peer_c", GRIDcreateTestPeerInfo("Charlie", "host3", 3000));
   GRIDaddTestPeer(&Node, "peer_b", GRIDcreateTestPeerInfo("Bravo", "host2", 2000));

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntry("peer_a", "Alpha"));
   Response.push_back(GRIDcreateTestPeerEntry("peer_c", "Charlie"));
   Response.push_back(GRIDcreateTestPeerEntry("peer_b", "Bravo"));

   GRIDsortPeers(&Response, &Node, "name", false);

   UNIT_ASSERT(Response[0]["name"].asString() == "Charlie");
   UNIT_ASSERT(Response[1]["name"].asString() == "Bravo");
   UNIT_ASSERT(Response[2]["name"].asString() == "Alpha");
}

static void testSortNameDuplicates() {
   COL_FUNCTION(testSortNameDuplicates);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDaddTestPeer(&Node, "peer_1", GRIDcreateTestPeerInfo("SameName", "host1", 1000));
   GRIDaddTestPeer(&Node, "peer_2", GRIDcreateTestPeerInfo("SameName", "host2", 2000));
   GRIDaddTestPeer(&Node, "peer_3", GRIDcreateTestPeerInfo("Unique", "host3", 3000));

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntry("peer_1", "SameName"));
   Response.push_back(GRIDcreateTestPeerEntry("peer_2", "SameName"));
   Response.push_back(GRIDcreateTestPeerEntry("peer_3", "Unique"));

   GRIDsortPeers(&Response, &Node, "name", true);

   UNIT_ASSERT(Response.size() == 3);
}

static void testSortStatusErrorsFirst() {
   COL_FUNCTION(testSortStatusErrorsFirst);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDaddTestPeer(&Node, "peer_err", GRIDcreateTestPeerInfoError("ErrorPeer", "host1", 1000, "Connection failed"));
   GRIDaddTestPeer(&Node, "peer_disc", GRIDcreateTestPeerInfo("DiscPeer", "host2", 2000));

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntry("peer_disc", "DiscPeer"));
   Response.push_back(GRIDcreateTestPeerEntry("peer_err", "ErrorPeer"));

   GRIDsortPeers(&Response, &Node, "status", false);

   UNIT_ASSERT(Response[0]["id"].asString() == "peer_err");
   UNIT_ASSERT(Response[1]["id"].asString() == "peer_disc");
}

static void testSortStatusDisconnectedBeforeConnected() {
   COL_FUNCTION(testSortStatusDisconnectedBeforeConnected);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDaddTestPeer(&Node, "peer_a", GRIDcreateTestPeerInfo("PeerA", "host1", 1000));
   GRIDaddTestPeer(&Node, "peer_b", GRIDcreateTestPeerInfo("PeerB", "host2", 2000));
   GRIDaddTestPeer(&Node, "peer_err", GRIDcreateTestPeerInfoError("ErrorPeer", "host3", 3000, "Timeout"));

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntry("peer_a", "PeerA"));
   Response.push_back(GRIDcreateTestPeerEntry("peer_err", "ErrorPeer"));
   Response.push_back(GRIDcreateTestPeerEntry("peer_b", "PeerB"));

   GRIDsortPeers(&Response, &Node, "status", false);

   UNIT_ASSERT(Response[0]["id"].asString() == "peer_err");
   UNIT_ASSERT(Response[1]["id"].asString() == "peer_a" || Response[1]["id"].asString() == "peer_b");
   UNIT_ASSERT(Response[2]["id"].asString() == "peer_a" || Response[2]["id"].asString() == "peer_b");
}

static void testSortStatusAscending() {
   COL_FUNCTION(testSortStatusAscending);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDaddTestPeer(&Node, "peer_err", GRIDcreateTestPeerInfoError("ErrorPeer", "host1", 1000, "Failed"));
   GRIDaddTestPeer(&Node, "peer_disc", GRIDcreateTestPeerInfo("DiscPeer", "host2", 2000));

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntry("peer_err", "ErrorPeer"));
   Response.push_back(GRIDcreateTestPeerEntry("peer_disc", "DiscPeer"));

   GRIDsortPeers(&Response, &Node, "status", true);

   UNIT_ASSERT(Response[0]["id"].asString() == "peer_disc");
   UNIT_ASSERT(Response[1]["id"].asString() == "peer_err");
}

static void testSortEmptyResponse() {
   COL_FUNCTION(testSortEmptyResponse);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   COLvar Response;
   Response.setArrayType();

   GRIDsortPeers(&Response, &Node, "name", true);
   GRIDsortPeers(&Response, &Node, "status", false);

   UNIT_ASSERT(Response.size() == 0);
}

static void testSortSingleItem() {
   COL_FUNCTION(testSortSingleItem);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDaddTestPeer(&Node, "peer_1", GRIDcreateTestPeerInfo("Only", "host1", 1000));

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntry("peer_1", "Only"));

   GRIDsortPeers(&Response, &Node, "name", true);

   UNIT_ASSERT(Response[0]["id"].asString() == "peer_1");
}

static void testSortDefault() {
   COL_FUNCTION(testSortDefault);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDaddTestPeer(&Node, "peer_z", GRIDcreateTestPeerInfoError("Zebra", "host1", 1000, "Connection error"));
   GRIDaddTestPeer(&Node, "peer_a", GRIDcreateTestPeerInfo("Apple", "host2", 2000));

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntry("peer_z", "Zebra"));
   Response.push_back(GRIDcreateTestPeerEntry("peer_a", "Apple"));

   GRIDsortPeers(&Response, &Node, "unknown_sort", true);

   UNIT_ASSERT(Response[0]["name"].asString() == "Apple");
   UNIT_ASSERT(Response[1]["name"].asString() == "Zebra");
}

static void testSortLargeDataset() {
   COL_FUNCTION(testSortLargeDataset);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   for(int i = 0; i < 50; i++) {
      COLstring Id   = "peer_" + COLintToString(i);
      COLstring Name = "Name_" + COLintToString(100 - i);
      GRIDaddTestPeer(&Node, Id, GRIDcreateTestPeerInfo(Name, "host", i));
   }

   COLvar Response;
   Response.setArrayType();
   for(int i = 0; i < 50; i++) {
      COLstring Id = "peer_" + COLintToString(i);
      Response.push_back(GRIDcreateTestPeerEntry(Id, "Name_" + COLintToString(100 - i)));
   }

   GRIDsortPeers(&Response, &Node, "name", true);

   UNIT_ASSERT(Response[0]["name"].asString() == "Name_100");
   UNIT_ASSERT(Response[49]["name"].asString() == "Name_99");
}

static void testSortMixedStates() {
   COL_FUNCTION(testSortMixedStates);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDaddTestPeer(&Node, "p1", GRIDcreateTestPeerInfoError("P1", "host1", 1000, "Error1"));
   GRIDaddTestPeer(&Node, "p2", GRIDcreateTestPeerInfoError("P2", "host2", 2000, "Error2"));
   GRIDaddTestPeer(&Node, "p3", GRIDcreateTestPeerInfo("P3", "host3", 3000));
   GRIDaddTestPeer(&Node, "p4", GRIDcreateTestPeerInfo("P4", "host4", 4000));

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntry("p4", "P4"));
   Response.push_back(GRIDcreateTestPeerEntry("p1", "P1"));
   Response.push_back(GRIDcreateTestPeerEntry("p3", "P3"));
   Response.push_back(GRIDcreateTestPeerEntry("p2", "P2"));

   GRIDsortPeers(&Response, &Node, "status", false);

   UNIT_ASSERT(Response[0]["id"].asString() == "p1" || Response[0]["id"].asString() == "p2");
   UNIT_ASSERT(Response[1]["id"].asString() == "p1" || Response[1]["id"].asString() == "p2");
   UNIT_ASSERT(Response[2]["id"].asString() == "p3" || Response[2]["id"].asString() == "p4");
   UNIT_ASSERT(Response[3]["id"].asString() == "p3" || Response[3]["id"].asString() == "p4");
}

static void testSortErrorsAscending() {
   COL_FUNCTION(testSortErrorsAscending);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDaddTestPeer(&Node, "peer_high", GRIDcreateTestPeerInfoWithMetrics("High", "host1", 1000, 10, 5, 100));
   GRIDaddTestPeer(&Node, "peer_low", GRIDcreateTestPeerInfoWithMetrics("Low", "host2", 2000, 2, 5, 100));
   GRIDaddTestPeer(&Node, "peer_mid", GRIDcreateTestPeerInfoWithMetrics("Mid", "host3", 3000, 5, 5, 100));

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntryWithMetrics("peer_high", "High", 10, 5, 100));
   Response.push_back(GRIDcreateTestPeerEntryWithMetrics("peer_low", "Low", 2, 5, 100));
   Response.push_back(GRIDcreateTestPeerEntryWithMetrics("peer_mid", "Mid", 5, 5, 100));

   GRIDsortPeers(&Response, &Node, "errors", true);

   UNIT_ASSERT(Response[0]["id"].asString() == "peer_low");
   UNIT_ASSERT(Response[1]["id"].asString() == "peer_mid");
   UNIT_ASSERT(Response[2]["id"].asString() == "peer_high");
}

static void testSortErrorsDescending() {
   COL_FUNCTION(testSortErrorsDescending);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDaddTestPeer(&Node, "peer_low", GRIDcreateTestPeerInfoWithMetrics("Low", "host1", 1000, 2, 5, 100));
   GRIDaddTestPeer(&Node, "peer_high", GRIDcreateTestPeerInfoWithMetrics("High", "host2", 2000, 10, 5, 100));

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntryWithMetrics("peer_low", "Low", 2, 5, 100));
   Response.push_back(GRIDcreateTestPeerEntryWithMetrics("peer_high", "High", 10, 5, 100));

   GRIDsortPeers(&Response, &Node, "errors", false);

   UNIT_ASSERT(Response[0]["id"].asString() == "peer_high");
   UNIT_ASSERT(Response[1]["id"].asString() == "peer_low");
}

static void testSortQueueAscending() {
   COL_FUNCTION(testSortQueueAscending);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDaddTestPeer(&Node, "peer_high", GRIDcreateTestPeerInfoWithMetrics("High", "host1", 1000, 0, 100, 50));
   GRIDaddTestPeer(&Node, "peer_low", GRIDcreateTestPeerInfoWithMetrics("Low", "host2", 2000, 0, 10, 50));
   GRIDaddTestPeer(&Node, "peer_mid", GRIDcreateTestPeerInfoWithMetrics("Mid", "host3", 3000, 0, 50, 50));

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntryWithMetrics("peer_high", "High", 0, 100, 50));
   Response.push_back(GRIDcreateTestPeerEntryWithMetrics("peer_low", "Low", 0, 10, 50));
   Response.push_back(GRIDcreateTestPeerEntryWithMetrics("peer_mid", "Mid", 0, 50, 50));

   GRIDsortPeers(&Response, &Node, "queue", true);

   UNIT_ASSERT(Response[0]["id"].asString() == "peer_low");
   UNIT_ASSERT(Response[1]["id"].asString() == "peer_mid");
   UNIT_ASSERT(Response[2]["id"].asString() == "peer_high");
}

static void testSortThroughputDescending() {
   COL_FUNCTION(testSortThroughputDescending);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDaddTestPeer(&Node, "peer_low", GRIDcreateTestPeerInfoWithMetrics("Low", "host1", 1000, 0, 5, 50));
   GRIDaddTestPeer(&Node, "peer_high", GRIDcreateTestPeerInfoWithMetrics("High", "host2", 2000, 0, 5, 500));
   GRIDaddTestPeer(&Node, "peer_mid", GRIDcreateTestPeerInfoWithMetrics("Mid", "host3", 3000, 0, 5, 250));

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntryWithMetrics("peer_low", "Low", 0, 5, 50));
   Response.push_back(GRIDcreateTestPeerEntryWithMetrics("peer_high", "High", 0, 5, 500));
   Response.push_back(GRIDcreateTestPeerEntryWithMetrics("peer_mid", "Mid", 0, 5, 250));

   GRIDsortPeers(&Response, &Node, "throughput", false);

   UNIT_ASSERT(Response[0]["id"].asString() == "peer_high");
   UNIT_ASSERT(Response[1]["id"].asString() == "peer_mid");
   UNIT_ASSERT(Response[2]["id"].asString() == "peer_low");
}

static void testSortMetricsWithDefaults() {
   COL_FUNCTION(testSortMetricsWithDefaults);
   COLthreadPool Pool;
   Pool.start();
   SCKloop    Loop(&Pool);
   GRIDconfig Config = GRIDcreateTestConfig("node1");
   GRIDnode   Node(Config, Loop);

   GRIDpeerInfo InfoWithMetrics = GRIDcreateTestPeerInfoWithMetrics("WithMetrics", "host1", 1000, 5, 10, 100);
   GRIDpeerInfo InfoNoMetrics   = GRIDcreateTestPeerInfo("NoMetrics", "host2", 2000);
   InfoNoMetrics.Metrics.CompsWithErr = -1;
   InfoNoMetrics.Metrics.MaxQueueCount = -1;
   InfoNoMetrics.Metrics.Throughput = -1;

   GRIDaddTestPeer(&Node, "peer_with", InfoWithMetrics);
   GRIDaddTestPeer(&Node, "peer_without", InfoNoMetrics);

   COLvar Response;
   Response.setArrayType();
   Response.push_back(GRIDcreateTestPeerEntryWithMetrics("peer_with", "WithMetrics", 5, 10, 100));
   Response.push_back(GRIDcreateTestPeerEntryWithMetrics("peer_without", "NoMetrics", -1, -1, -1));

   GRIDsortPeers(&Response, &Node, "errors", false);

   UNIT_ASSERT(Response[0]["id"].asString() == "peer_with");
   UNIT_ASSERT(Response[1]["id"].asString() == "peer_without");
}

void testSort(UNITapp* pApp) {
   pApp->add("sort/name/ascending",        &testSortNameAscending);
   pApp->add("sort/name/descending",       &testSortNameDescending);
   pApp->add("sort/name/duplicates",       &testSortNameDuplicates);
   pApp->add("sort/status/errors_first",   &testSortStatusErrorsFirst);
   pApp->add("sort/status/disconnected",   &testSortStatusDisconnectedBeforeConnected);
   pApp->add("sort/status/ascending",      &testSortStatusAscending);
   pApp->add("sort/errors/ascending",      &testSortErrorsAscending);
   pApp->add("sort/errors/descending",     &testSortErrorsDescending);
   pApp->add("sort/queue/ascending",       &testSortQueueAscending);
   pApp->add("sort/throughput/descending", &testSortThroughputDescending);
   pApp->add("sort/metrics/defaults",      &testSortMetricsWithDefaults);
   pApp->add("sort/empty",                 &testSortEmptyResponse);
   pApp->add("sort/single",                &testSortSingleItem);
   pApp->add("sort/default",               &testSortDefault);
   pApp->add("sort/large",                 &testSortLargeDataset);
   pApp->add("sort/mixed",                 &testSortMixedStates);
}
