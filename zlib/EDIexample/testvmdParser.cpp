// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testvmdParser.cpp
//
//  Description
//
//  Author: Matthew Sobkowski
//  Date:   10/5/22 9:58 AM
// ---------------------------------------------------------------------------
#include "testvmdParser.h"

#include <UNIT/UNITapp.h>
#include <FIL/FILutils.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIload.h>
#include <EDI/EDIsave.h>
#include <EDI/EDIvmdParser.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testSimpleX12Vmd() {
   COL_FUNCTION(testSimpleX12Vmd);
   COLstring Content;
   FILreadFile("x12/simple_x12.vmd", &Content);
   EDIcollection Collection;
   COLstring ConversionErrors;
   EDItranslateVmd(Content, &Collection, &ConversionErrors);
   COLstring Out, Err;
   EDIsave(Collection, &Out, &Err);
   FILwriteFile("x12/simple_x12_new.vmd", Out);
   bool isLegacy;
   EDIcollection UpdatedCollection;
   EDIsmartLoad(Out, &UpdatedCollection, &Err, &ConversionErrors, &isLegacy);

   for(const auto& it : Collection.Nodes) {
      UNIT_ASSERT(UpdatedCollection.Nodes.count(it->first));
      int OldChild = it->second->Children.size();
      int NewChild = UpdatedCollection.Nodes[it->first]->Children.size();
      UNIT_ASSERT_MESSAGE(OldChild == NewChild, it->first << " " << OldChild << " != " << NewChild);
   }
}

static void testSimpleHl7Vmd() {
   COL_FUNCTION(testSimpleHl7Vmd);
   COLstring Content;
   FILreadFile("simple.vmd", &Content);
   EDIcollection Collection;
   COLstring ConversionErrors;
   EDItranslateVmd(Content, &Collection, &ConversionErrors);
   COLstring Out, Err;
   EDIsave(Collection, &Out, &Err);
   bool isLegacy;
   EDIcollection UpdatedCollection;
   EDIsmartLoad(Out, &UpdatedCollection, &Err, &ConversionErrors, &isLegacy);

   for(const auto& it : Collection.Nodes) {
      UNIT_ASSERT(UpdatedCollection.Nodes.count(it->first));
      UNIT_ASSERT_EQUALS(it->second->Children.size(), UpdatedCollection.Nodes[it->first]->Children.size());
   }

}

static void testVmdParseRules(){
   COL_FUNCTION(testVmdParseRules);
   COLstring DemoVmd;
   FILreadFile("hl7/load.vmd", &DemoVmd);
   EDIcollection Collection;
   COLstring ConversionErrors;
   EDItranslateVmd(DemoVmd, &Collection, &ConversionErrors);
   UNIT_ASSERT_EQUALS(Collection.MatchRules.size(), 2);
   UNIT_ASSERT_EQUALS(Collection.MatchRules[0].Match, "ADT*");
   UNIT_ASSERT_EQUALS(Collection.MatchRules[0].MessageName, "ADT");
   UNIT_ASSERT_EQUALS(Collection.MatchRules[1].Match, "*");
   UNIT_ASSERT_EQUALS(Collection.MatchRules[1].MessageName, "Catchall");
}

static void testSimpleRegexMatch() {
   COL_FUNCTION(testSimpleRegexMatch);
   COLstring DemoVmd;
   FILreadFile("hl7/ela.vmd", &DemoVmd);
   EDIcollection Collection;
   COLstring ConversionErrors;
   EDItranslateVmd(DemoVmd, &Collection, &ConversionErrors);
   UNIT_ASSERT_EQUALS(Collection.MatchRules.size(), 7);
   UNIT_ASSERT_EQUALS(Collection.MatchRules[0].Match, "ADT*");
   UNIT_ASSERT_EQUALS(Collection.MatchRules[1].Match, "ORM*");
   UNIT_ASSERT_EQUALS(Collection.MatchRules[2].Match, "BAR*");
   UNIT_ASSERT_EQUALS(Collection.MatchRules[3].Match, "RDE*");
   UNIT_ASSERT_EQUALS(Collection.MatchRules[4].Match, "DFT*");
   UNIT_ASSERT_EQUALS(Collection.MatchRules[5].Match, "CHG*");
   UNIT_ASSERT_EQUALS(Collection.MatchRules[6].Match, "*");
}

static void testRegexAck() {
   COL_FUNCTION(testRegexAck);
   COLstring DemoVmd;
   FILreadFile("hl7/ack.vmd", &DemoVmd);
   EDIcollection Collection;
   COLstring ConversionErrors;
   EDItranslateVmd(DemoVmd, &Collection, &ConversionErrors);
   UNIT_ASSERT_EQUALS(Collection.MatchRules.size(), 1);
   UNIT_ASSERT_EQUALS(Collection.MatchRules[0].Match, "ACK");
}

static void testRegexAdtA01() {
   COL_FUNCTION(testRegexAdtA01);
   COLstring DemoVmd;
   FILreadFile("hl7/adt_a01.vmd", &DemoVmd);
   EDIcollection Collection;
   COLstring ConversionErrors;
   EDItranslateVmd(DemoVmd, &Collection, &ConversionErrors);
   UNIT_ASSERT_EQUALS(Collection.MatchRules.size(), 1);
   UNIT_ASSERT_EQUALS(Collection.MatchRules[0].Match, "ADT^A01*");
}

static void testNameCollision() {
   COL_FUNCTION(testNameCollision);
   // If a message definition has the same name as a segment definition,
   // rename the message definition to <name>_msg_1
   COLstring DemoVmd;
   FILreadFile("hl7/name_collision.vmd", &DemoVmd);
   EDIcollection Collection;
   COLstring ConversionErrors;
   EDItranslateVmd(DemoVmd, &Collection, &ConversionErrors);
   UNIT_ASSERT_EQUALS(16,          Collection.MatchRules.size());
   UNIT_ASSERT_EQUALS(EDI_SEGMENT, Collection.Nodes["ZPM"]->Type);
   UNIT_ASSERT_EQUALS(EDI_MESSAGE, Collection.Nodes["ZPM_msg_1"]->Type);
   UNIT_ASSERT_EQUALS("ZPM*",      Collection.MatchRules[14].Match);
}

static void testDuplicateSegmentGroups(){
   COL_FUNCTION(testDuplicateSegmentGroups);
   // This vmd has multiple message definitions which use segment groups that
   // have the same name, but potentially different structures.
   COLstring Vmd;
   FILreadFile("hl7/duplicate_segment_groups.vmd", &Vmd);
   EDIcollection Collection;
   COLstring ConversionErrors;
   EDItranslateVmd(Vmd, &Collection, &ConversionErrors);
   for (const auto& it : Collection.Nodes){
      COL_VAR2(it->first, it->second->Name);
   }
}

void testVmdParser(UNITapp* pApp){
   pApp->add("vmdParser/parse/rules", &testVmdParseRules);
   pApp->add("vmdParser/simple_hl7",  &testSimpleHl7Vmd);
   pApp->add("vmdParser/regex",       &testSimpleRegexMatch);
   pApp->add("vmdParser/ack",         &testRegexAck);
   pApp->add("vmdParser/adt_a01",     &testRegexAdtA01);
   pApp->add("vmdParser/same/name",   &testNameCollision);
   pApp->add("vmdParser/segment_groups/duplicates",   &testDuplicateSegmentGroups);
//   pApp->add("vmdParser/simple_x12",  &testSimpleX12Vmd);
}