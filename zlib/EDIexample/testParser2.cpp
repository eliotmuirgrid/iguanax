// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testParser.cpp
//
//  Description:
//  Implementation
//
//  Author: Vismay Shah
//  Date:   Tue 01 Nov 2022 09:25 EDT
// ---------------------------------------------------------------------------
#include "testParser2.h"

#include <COL/COLlog.h>
#include <COL/COLhashmap.h>
#include <COL/COLvar.h>
#include <EDI/EDIdata.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIhl7.h>
#include <EDI/EDIload.h>
#include <EDI/EDIparse.h>
#include <FIL/FILutils.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

// Creates a simple grammar with just MSH and PID, all part of an ADTA01 identity.
static EDIcollection* simpleGrammar(){  // creates grammar on the fly.
   COL_FUNCTION(simpleGrammar);
   EDIcollection* pGrammar = new EDIcollection(); // caller takes responsibility to clean up EDIcollection*
   pGrammar->Nodes.add("NM", new EDInode("NM", "Name", EDI_COMPOSITE));
   pGrammar->Nodes.add("ST", new EDInode("ST", "String", EDI_COMPOSITE));
   pGrammar->Nodes.add("PAT", new EDInode("PAT", "Patient", EDI_COMPOSITE));
   pGrammar->Nodes.add("MSG", new EDInode("MSG", "MessageType", EDI_COMPOSITE));

   pGrammar->Nodes.add("PID", new EDInode("PID", "Patient IDentification Segment", EDI_SEGMENT));
   pGrammar->Nodes.add("MSH", new EDInode("MSH", "Message Header Segment", EDI_SEGMENT));
   pGrammar->Nodes.add("ADTA01", new EDInode("ADTA01", "Patient admit message", EDI_MESSAGE));
   pGrammar->Nodes["NM"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "ST", "Surname"));
   pGrammar->Nodes["NM"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "ST", "First Name"));
   pGrammar->Nodes["NM"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "ST", "Middle Name"));
   pGrammar->Nodes["MSG"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "ST", "Type"));
   pGrammar->Nodes["MSG"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "ST", "Event"));
   pGrammar->Nodes["PID"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "Identifier", false, true));
   pGrammar->Nodes["PID"]->Children.push_back(EDIchild(pGrammar->Nodes["NM"], "Patient Name", false, true));
   pGrammar->Nodes["PID"]->Children.push_back(EDIchild(pGrammar->Nodes["PAT"], "Patient Team", false, false));
   pGrammar->Nodes["MSH"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"],  "Time"));
   pGrammar->Nodes["MSH"]->Children.push_back(EDIchild(pGrammar->Nodes["MSG"], "Type"));

   pGrammar->Nodes["ADTA01"]->Children.push_back(EDIchild(pGrammar->Nodes["MSG"],  "Message header segment"));
   pGrammar->Nodes["ADTA01"]->Children.push_back(EDIchild(pGrammar->Nodes["PID"],  "Patient IDentification segment", false, false));
   return pGrammar;
}

static EDIcollection* loadCollection(COLstring VmdFile){
   COL_FUNCTION(loadCollection);
   FILcorrectPathSeparators(VmdFile);
   COLstring Vmd;
   COL_TRC("Loading file");
   if (FILfileExists(VmdFile)){ FILreadFile(VmdFile, &Vmd); }
   else                       { COL_TRC("File not found");  }
   EDIcollection* pCollection = new EDIcollection(); // caller takes responsibility to clean up EDIcollection*
   COL_TRC("Constructing collection");
   COLstring Error;
   bool IsLegacy = false;
   COLstring ConversionErrors;
   EDIsmartLoad(Vmd, pCollection, &Error, &ConversionErrors, &IsLegacy);
   return pCollection;
};

static COLstring loadResultJson(COLstring ExpectedOutputFile){
   COL_FUNCTION(loadResultJson);
   FILcorrectPathSeparators(ExpectedOutputFile);
   COLstring ResultJson;
   COL_TRC("Loading file");
   if (FILfileExists(ExpectedOutputFile)){ FILreadFile(ExpectedOutputFile, &ResultJson); }
   else                                  { COL_TRC("File not found");                    }
   return ResultJson;
}

static void parseTester(EDIdata* pNode, EDInode* pGrammar, COLstring ResultJson) {
   COL_FUNCTION(parseTester);
   COLvar NodeAsVar;
   pNode->toVar(&NodeAsVar);
   COLstring NodeJson = NodeAsVar.json();
   COL_DUMP("NodeJson", NodeJson.c_str(), NodeJson.size());
   UNIT_ASSERT_EQUALS(NodeJson, ResultJson);
}

static void parseResultTester(EDIdata* pNode, EDInode* pGrammar, COLstring ParsedResult){
   COL_FUNCTION(parseResultTester);
   COLstring Parsed;
   pNode->flatwire(&Parsed);
   COL_DUMP("Parsed", Parsed.c_str(), Parsed.size());
   COL_DUMP("Real", ParsedResult.c_str(), ParsedResult.size());
   COL_VAR(Parsed.c_str());
   COL_VAR(ParsedResult.c_str());
   for (int i = 0; i < ParsedResult.size(); i++){
      if (Parsed[i] != ParsedResult[i]) { COL_VAR(i); UNIT_ASSERT(false); }
   }
   UNIT_ASSERT(true);
}

static void missingSegmentTester(EDIdata* pMessage, EDInode* pGrammar, COLlist<COLstring>* pExpected) {
   COL_FUNCTION(missingSegmentTester);
   COLvar Missing;
   COLlist<COLstring> MissingNodes;
   EDIfindMissingSegments(pMessage, pGrammar, MissingNodes, Missing);
   UNIT_ASSERT_EQUALS(pExpected->size(), MissingNodes.size());
   auto i = pExpected->begin();
   auto j = MissingNodes.begin();
   while (i != pExpected->end()) {
      UNIT_ASSERT_EQUALS(*i, *j);
      i++;
      j++;
   }
}

static void repeatCountTester(EDIdata* pMessage, EDInode* pGrammar, COLlist<COLstring>* pExpected) {
   COL_FUNCTION(repeatCountTester);
   COLvar NodeAsVar;
   pMessage->toVar(&NodeAsVar);
   COL_VAR(NodeAsVar.json(1));
   UNIT_ASSERT_EQUALS("MSH", NodeAsVar[0]["c"][0]["n"]);
   UNIT_ASSERT_EQUALS("OBX", NodeAsVar[0]["c"][1]["n"]); // this is the obx repeat node
   UNIT_ASSERT_EQUALS(2, NodeAsVar[0]["c"][1]["c"].size()); // this is the obx repeat, should have 2 obx segments
   UNIT_ASSERT_EQUALS(2, NodeAsVar[0]["c"].size()); // this is the size of the result, 1 MSH and 1 OBX GROUP with 2 OBX segments.
}

// static void msgParsedTester(EDIdata* pMessage, EDInode* pGrammer, COLstring Result){
//    COL_FUNCTION(msgParsedTester);
//    COLstring ParsedMessage;
//    pMessage->flatwire(&ParsedMessage);
//    ParsedMessage.replace("\\F\\|\\S\\\\R\\\\E\\\\T\\", "^~\\&");
//    COL_TRC_DUMP("Expected", Result.c_str(), 112);
//    COL_TRC_DUMP("Parsed Result", ParsedMessage.c_str(), 128);
//    UNIT_ASSERT_EQUALS(Result, ParsedMessage);
// }

// The general purpose helper - takes ownership and deletes the inputs.
static void testParserHelper(EDIcollection* pCollection, const COLstring& MsgName, const COLstring& Data, COLclosure2<EDIdata*, EDInode*>* pTester){
   COL_FUNCTION(testParserHelper);
   COL_DUMP("Message:", Data.c_str(), Data.size());
   if(!pCollection){
      COL_TRC("Failed to generate collection");
      UNIT_ASSERT(false);
   }
   if(Data.is_null()){
      COL_TRC("Failed to get Hl7 Message");
      delete pCollection;
      UNIT_ASSERT(false);
   }
   COLlist<COLstring> Skipped;
   // Use pCollection to parse message, and create the parsed tree
   EDIdata* pNode = new EDIdata(*pCollection, MsgName);
   COLarray<COLstring> Here;
   EDInode* pGrammar = pCollection->Nodes[MsgName];
   COLhashmap<COLstring, bool> MessageSegments;
   EDIgetSegmentsInMessage(pCollection, pGrammar, &MessageSegments);
   COL_VAR(MessageSegments.size());
   int Consumed = EDIparseSegmentCollection(pNode, Data.c_str(), Data.size(), MessageSegments, Skipped, &Here);
   COL_VAR(Consumed);
   EDIpostProcess(pNode); // Sets MSH separator char and chars to | and ^~\& respectively
   // run test
   pTester->runAndDelete(pNode, pGrammar);
   // clean up on-heap data
   delete pCollection;
   delete pNode;
}

static void testBasicVmd(){
   COL_FUNCTION(testBasicVmd);
   COLclosure2<EDIdata*, EDInode*>* pTester = COLnewClosure2(parseTester, loadResultJson("hl7/result/simple.hl7"));
   testParserHelper(loadCollection("hl7/simple.vmd"), "SimpleMessage1", "MSH|^~\\&|Epic|Iguana|ADT^A04|\rPID|FIRST|010|Mercury^Freddie\r", pTester);
}

static void testHl7EscapeSequences(){
   COL_FUNCTION(testHl7EscapeSequences);
   COLclosure2<EDIdata*, EDInode*>* pTester = COLnewClosure2(parseTester, loadResultJson("hl7/result/hl7escape.hl7"));
   testParserHelper(loadCollection("hl7/simple.vmd"), "SimpleMessage1", "MSH|^~\\&|Epic|Igu\\F\\\\F\\\\R\\ana|ADT^A04|\rPID|FIRST|010|Mercury^Fredd\\S\\\\T\\ie\r", pTester);
}

static void testShortSegment(){
   COL_FUNCTION(testShortSegment);
   COLclosure2<EDIdata*, EDInode*>* pTester = COLnewClosure2(parseTester, loadResultJson("hl7/result/hl7escape.hl7"));
   testParserHelper(loadCollection("hl7/simple.vmd"), "SimpleMessage1", "MSH|^~\\&|Epic|Igu\\F\\\\F\\\\R\\ana|ADT^A04|\rPID|FIRST|010|Mercury^Fredd\\S\\\\T\\ie", pTester);
}

static void testMissingSegment(){
   COL_FUNCTION(testMissingSegment);
   // adt_a01.vmd Message1 message requires the presence of MSH, EVN, PID, and PV1. The parse should warning about the missing segments.
   COLlist<COLstring> Expected;
   Expected.push_back("EVN");
   Expected.push_back("PID");
   Expected.push_back("PV1");
   // usually not a great idea to pass pointer to a stack variable, but I can cheat here because the variable outlives the closure.
   COLclosure2<EDIdata*, EDInode*>* pTester = COLnewClosure2(missingSegmentTester, &Expected);
   testParserHelper(loadCollection("hl7/adt_a01.vmd"), "Message1", "MSH|^~\\&|Epic|Igu\\F\\\\F\\\\R\\ana|ADT^A04|\r", pTester);
}

static void testparseHl7(){
   COL_FUNCTION(testparseHl7);
   COLstring Data, ErrorMsg;
   FILreadFile("hl7/msg1.txt", &Data);
   EDIhl7Preprocess(&Data, &ErrorMsg);
   COL_DUMP("Message:", Data.c_str(), Data.size());
   EDIcollection* pCollection = loadCollection("hl7/demo.vmd");
   COLstring Match = "ADT";
   bool Success = EDIparseMsh(*pCollection, &Data, &Match, &ErrorMsg);
   UNIT_ASSERT(Success);
   UNIT_ASSERT_EQUALS("ADT", Match);
   delete pCollection;
}

static void testMatchingOrder(){
   COL_FUNCTION(testMatchingOrder);
   // This vmd has 3 rules: ADT mapped to ADT*, Catchall mapped to * and Lab mapped to ORU*
   // with the mapping order [ADT, Lab, Catchall]
   EDIcollection* pCollection = loadCollection("hl7/multipleRules.vmd");
   UNIT_ASSERT_EQUALS(3, pCollection->MatchRules.size());
   UNIT_ASSERT_EQUALS("ADT",      pCollection->MatchRules[0].MessageName);
   UNIT_ASSERT_EQUALS("ADT*",     pCollection->MatchRules[0].Match);
   UNIT_ASSERT_EQUALS("Lab",      pCollection->MatchRules[1].MessageName);
   UNIT_ASSERT_EQUALS("ORU*",     pCollection->MatchRules[1].Match);
   UNIT_ASSERT_EQUALS("Catchall", pCollection->MatchRules[2].MessageName);
   UNIT_ASSERT_EQUALS("*",        pCollection->MatchRules[2].Match);
   // clean up on-heap data
   delete pCollection;
}

// Test for IX-3434. We have a simple vmd which defines a repeating OBX segment after MSH. The message string
// has a bunch of segments, including a ZID segment after the second OBX segment. We should see OBX having
// a repeat count of 2.
static void testRepeatCount(){
   COL_FUNCTION(testRepeatCount);
   COLstring Data, ErrorMsg;
   FILreadFile("hl7/ix3434.txt", &Data);
   EDIhl7Preprocess(&Data, &ErrorMsg);
   COLlist<COLstring> Expected;
   Expected.push_back("MSH");
   Expected.push_back("OBX");
   Expected.push_back("OBX");
   COLclosure2<EDIdata*, EDInode*>* pTester = COLnewClosure2(repeatCountTester, &Expected);
   testParserHelper(loadCollection("hl7/IX3434.vmd"), "ADTA01", Data, pTester);
}

// Tests to make sure the presence or absence of a | at the end of a segment dosen't affect the parsed message.
static void testSegementNoPipe(){
   COL_FUNCTION(testSegementNoPipe);
   COLstring MsgWithoutPipe = "MSH|^~\\&|EPIC|EPICADT|SMS|SMSADT|202211031408|CHARRIS|ADT^A04|1817457|D|2.5|\rEVN||202211030800||||202211030800\r";
   COLclosure2<EDIdata*, EDInode*>* pTester = COLnewClosure2(parseTester, loadResultJson("hl7/result/msg_evn_result.hl7"));
   testParserHelper(loadCollection("hl7/adt_a04.vmd"), "ADTA04", MsgWithoutPipe, pTester);
}

static void testSegementWithPipe(){ 
   COL_FUNCTION(testSegementWithPipe);
   COLstring MsgWithPipe    = "MSH|^~\\&|EPIC|EPICADT|SMS|SMSADT|202211031408|CHARRIS|ADT^A04|1817457|D|2.5|\rEVN||202211030800||||202211030800|\r";
   COLclosure2<EDIdata*, EDInode*>* pTester = COLnewClosure2(parseTester, loadResultJson("hl7/result/msg_evn_result.hl7"));
   testParserHelper(loadCollection("hl7/adt_a04.vmd"), "ADTA04", MsgWithPipe, pTester);
}

// See IX-4167 for full details.
// Iguana 6 supported vmds where sub-sub fields could be set to a grammar node with children. When data was assgined in such a case,
// The grammar was ignored and the sub-sub field was just given the value as string data. Iguana X's parser was stricter and did not allow
// this until IX-4167 was done. Prior to it, trying to parse a message with a problematic vmd would throw an error.
static void testLegacyGrammar(){
   COL_FUNCTION(testLegacyGrammar);
   COLstring Msg = "MSH|^~\\&|RAMSOFT||||||ADT^A01|383081|P|2.3.1||||||UNICODE UTF-8||\rEVN|||||^^^^^^^&&&&&&&&&&&&&&&&&&&&&&foo|\r";
   COLstring Expected = loadResultJson("hl7/result/legacy_grammar_result.hl7");
   Expected.replace('\n', '\r');
   COLclosure2<EDIdata*, EDInode*>* pTester = COLnewClosure2(parseResultTester, Expected);
   testParserHelper(loadCollection("hl7/legacy_grammar_ix4167.vmd"), "Message1", Msg, pTester);
}

// This vmd has 2 segment groups called 'Observation' with different structures.
// The first  is 'Observation' : OBR OBX
// The second is 'Observation' : NTE PID
// Our parser will append the index 1 to the second 'Observation' group
static void testSegmentGroupNameClash(){
   COL_FUNCTION(testSegmentGroupNameClash);
   EDIcollection* pLoaded = loadCollection("hl7/segmentGroupVmds/SameGroupNameDiffContent.vmd");
   for (const auto& it : pLoaded->Nodes){
      if (it->second->Type == EDI_SEGMENT_GROUP){
         COL_TRC("KEY: " << it->first);
         COL_TRC("VALUE: " << it->second->Name);
      }
   }
   delete pLoaded;
}

// This vmd has 2 messages defined. The messages share a segment group in common and have a segment group unique to them
// Message1 : MSH {GRP1} {M1GRP2}
// Message2 : MSH {GRP1} {M2GRP2}
static void testSegmentGroupShared(){
   COL_FUNCTION(testSegmentGroupShared);
   EDIcollection* pLoaded = loadCollection("hl7/segmentGroupVmds/TwoMessages.vmd");
   for (const auto& it : pLoaded->Nodes){
      if (it->second->Type == EDI_SEGMENT_GROUP){
         COL_TRC("KEY: " << it->first);
         COL_TRC("VALUE: " << it->second->Name);
      }
   }
   delete pLoaded;
}

static void testMshSeparatorsTester(EDIdata* pMessage, EDInode* pGrammar){
   COL_FUNCTION(testMshSeparatorsTester);
   UNIT_ASSERT_EQUALS("|",     (*pMessage)[0][0].value());   // MSH-1
   UNIT_ASSERT_EQUALS("^~\\&", (*pMessage)[0][1].value());   // MSH-2
}

static void testMshSeparators(){
   COL_FUNCTION(testMshSeparators);
   COLstring Msg = "MSH|^~\\&|ORDERING_SYSTEM|HOSPITAL|LAB_SYSTEM|LAB|20260217103000||ORM^O01|123456|P|2.5\r";
   COLstring Error;
   UNIT_ASSERT(EDIhl7Preprocess(&Msg, &Error));
   UNIT_ASSERT_EQUALS("MSH|||", Msg.substr(0, 6)); // confirms preprocess stripped MSH-1/MSH-2
   COLclosure2<EDIdata*, EDInode*>* pTester = COLnewClosure2(testMshSeparatorsTester);
   testParserHelper(loadCollection("hl7/demo.vmd"), "Catchall", Msg, pTester);
}

void testParser2(UNITapp* pApp){
   pApp->add("parse/hl7",            &testparseHl7);
   pApp->add("parse/simple/message", &testBasicVmd);
   pApp->add("parse/Hl7escape",      &testHl7EscapeSequences);
   pApp->add("parse/matching_order", &testMatchingOrder);
   pApp->add("parse/segment/short",  &testShortSegment);
   pApp->add("parse/repeat/count",   &testRepeatCount);
   pApp->add("parse/segment/withPipe", &testSegementWithPipe);
   pApp->add("parse/segment/noPipe",   &testSegementNoPipe);
   pApp->add("parse/segment/msh_separators", &testMshSeparators);
   // pApp->add("parse/subsubfield/legacy_grammar", &testLegacyGrammar);
   // pApp->add("parse/segment/missing",&testMissingSegment);
   pApp->add("parse/segment_group/name_clash", &testSegmentGroupNameClash);
   pApp->add("parse/segment_group/shared",     &testSegmentGroupShared);
   
}
