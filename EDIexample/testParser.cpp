// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testParser.cpp
//
//  Description
//
//  Author: Eliot Muir 
//  Date:   Wed  5 Oct 2022 20:45:01 EDT
// ---------------------------------------------------------------------------
#include "testParser.h"

#include <UNIT/UNITapp.h>

#include <EDI/EDIgrammar.h>
#include <EDI/EDIdata.h>
#include <EDI/EDIhl7.h>
#include <EDI/EDIparse.h>
#include <EDI/EDIparseField.h>
#include <EDI/EDIload.h>
#include <FIL/FILutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

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

static void createEmptyGrammar(EDIcollection* pGrammar) {
   COL_FUNCTION(createEmptyGrammar);
   pGrammar->Nodes.add("ADTA01", new EDInode("ADTA01", "Patient admit message", EDI_MESSAGE));
}

static void createNameGrammar(EDIcollection* pGrammar){
   COL_FUNCTION(createNameGrammar);
   pGrammar->Nodes.add("NM", new EDInode("NM", "Name", EDI_COMPOSITE));
   pGrammar->Nodes.add("ST", new EDInode("ST", "String", EDI_COMPOSITE));
   pGrammar->Nodes.add("PAT", new EDInode("PAT", "Patient", EDI_COMPOSITE));
   pGrammar->Nodes.add("MSG", new EDInode("MSG", "MessageType", EDI_COMPOSITE));

   pGrammar->Nodes.add("PID", new EDInode("PID", "Patient IDentification Segment", EDI_SEGMENT));
   pGrammar->Nodes.add("MSH", new EDInode("MSH", "Message Header Segment", EDI_SEGMENT));
   pGrammar->Nodes.add("ADTA01", new EDInode("ADTA01", "Patient admit message", EDI_MESSAGE));
   pGrammar->Nodes.add("NK1", new EDInode("NK1", "Next of kin", EDI_SEGMENT));
   pGrammar->Nodes.add("PDA", new EDInode("PDA", "Patient death information", EDI_SEGMENT));
   pGrammar->Nodes.add("IN1", new EDInode("IN1", "Insurance segment", EDI_SEGMENT));
   pGrammar->Nodes.add("IN2", new EDInode("IN2", "Insurance segment", EDI_SEGMENT));
   pGrammar->Nodes.add("INSURANCE", new EDInode("INSURANCE", "Insurance segment group", EDI_SEGMENT_GROUP));
   pGrammar->Nodes["NM"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "Surname"));
   pGrammar->Nodes["NM"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "First Name"));
   pGrammar->Nodes["NM"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "Middle Name"));
   pGrammar->Nodes["PAT"]->Children.push_back(EDIchild(pGrammar->Nodes["NM"], "Patient"));
   pGrammar->Nodes["PAT"]->Children.push_back(EDIchild(pGrammar->Nodes["NM"], "Doctor"));
   pGrammar->Nodes["PAT"]->Children.push_back(EDIchild(pGrammar->Nodes["NM"], "Spouse"));
   pGrammar->Nodes["MSG"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "Type"));
   pGrammar->Nodes["MSG"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "Event"));
   pGrammar->Nodes["PID"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "Identifier", false, true));
   pGrammar->Nodes["PID"]->Children.push_back(EDIchild(pGrammar->Nodes["NM"], "Patient Name", false, true));
   pGrammar->Nodes["PID"]->Children.push_back(EDIchild(pGrammar->Nodes["PAT"], "Patient Team", false, false));
   pGrammar->Nodes["MSH"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "Time"));
   pGrammar->Nodes["MSH"]->Children.push_back(EDIchild(pGrammar->Nodes["MSG"], "Type"));
   pGrammar->Nodes["IN1"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "ID number"));
   pGrammar->Nodes["IN2"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "ID number"));
   pGrammar->Nodes["NK1"]->Children.push_back(EDIchild(pGrammar->Nodes["NM"], "Name of kin", false, false));
   pGrammar->Nodes["INSURANCE"]->Children.push_back(EDIchild(pGrammar->Nodes["IN1"], "Insurance segment 1"));
   pGrammar->Nodes["INSURANCE"]->Children.push_back(EDIchild(pGrammar->Nodes["IN2"], "Insurance segment 1"));

   pGrammar->Nodes["ADTA01"]->Children.push_back(EDIchild(pGrammar->Nodes["MSH"], "Message header segment"));
   pGrammar->Nodes["ADTA01"]->Children.push_back(EDIchild(pGrammar->Nodes["PID"], "Patient IDentification segment", false, false));
   pGrammar->Nodes["ADTA01"]->Children.push_back(EDIchild(pGrammar->Nodes["NK1"], "La family", true, true));   // repeating segments
   pGrammar->Nodes["ADTA01"]->Children.push_back(EDIchild(pGrammar->Nodes["PDA"], "Death info", true, false));
   pGrammar->Nodes["ADTA01"]->Children.push_back(EDIchild(pGrammar->Nodes["INSURANCE"], "Insurance Group", true, true));   // repeating segment group
}

static void parseSubField(){
   COL_FUNCTION(parseSubField);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("ADT&A01&BADDY");
   EDIparseSubField(&Message["MSH"][1], Field.c_str(), Field.size(), &Here);
   UNIT_ASSERT_EQUALS(2, Message["MSH"][1].size());
   UNIT_ASSERT_EQUALS(2, Message["MSH"][1].grammarSize());
   UNIT_ASSERT_EQUALS("ADT", Message["MSH"][1][0].value());
   UNIT_ASSERT_EQUALS("A01", Message["MSH"][1][1].value());
}

static void parseEmptySubField(){
   COL_FUNCTION(parseEmptySubField);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field; // empty
   EDIparseSubField(&Message["MSH"][1], Field.c_str(), Field.size(), &Here);
   UNIT_ASSERT_EQUALS(0, Message["MSH"][1].size()); 
   UNIT_ASSERT_EQUALS(2, Message["MSH"][1].grammarSize());
}

static void parseSingleSubField(){
   COL_FUNCTION(parseSingleSubField);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("ADT"); 
   EDIparseSubField(&Message["MSH"][1], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["MSH"][1]);
   UNIT_ASSERT_EQUALS(1, Message["MSH"][1].size());
   UNIT_ASSERT_EQUALS("ADT", Message["MSH"][1][0].value());
}

static void parseMissingSubField(){
   COL_FUNCTION(parseMissingSubField);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("ADT&"); // TODO: This means "ADT&" --> "ADT", "" - is this correct to parse as an empty string or should this raise an error
   EDIparseSubField(&Message["MSH"][1], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["MSH"][1]);
   UNIT_ASSERT_EQUALS(2, Message["MSH"][1].size());
   UNIT_ASSERT_EQUALS("ADT", Message["MSH"][1][0].value());
   UNIT_ASSERT_EQUALS("", Message["MSH"][1][1].value());
}

static void parseMultipleEmptySubField(){
   COL_FUNCTION(parseMultipleEmptySubField);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("&&"); 
   EDIparseSubField(&Message["MSH"][1], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["MSH"][1]);
   UNIT_ASSERT_EQUALS(2, Message["MSH"][1].grammarSize());
   UNIT_ASSERT_EQUALS(2, Message["MSH"][1].size());
   UNIT_ASSERT_EQUALS("", Message["MSH"][1][0].value());
   UNIT_ASSERT_EQUALS("", Message["MSH"][1][1].value());
}

static void parseMoreSubFieldThanGrammar(){
   COL_FUNCTION(parseMoreSubFieldThanGrammar);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("ADT&A01&&&End"); 
   EDIparseSubField(&Message["MSH"][1], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["MSH"][1]);
   UNIT_ASSERT_EQUALS(2, Message["MSH"][1].grammarSize());
   UNIT_ASSERT_EQUALS(2, Message["MSH"][1].size());
   UNIT_ASSERT_EQUALS("ADT", Message["MSH"][1][0].value());
   UNIT_ASSERT_EQUALS("A01", Message["MSH"][1][1].value());
}

static void parseField(){
   COL_FUNCTION(parseField);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("Muir&Eliot^Shah&Vismay^Smith&John");
   COL_VAR(Message["PID"][2].grammarSize());
   COL_VAR(Message["PID"][2][0].grammarSize());
   EDIparseField(&Message["PID"][2], Field.c_str(), Field.size(), &Here);
}

static void parseSingleField(){
   COL_FUNCTION(parseSingleField);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("Muir&Eliot");
   COL_VAR(Message["PID"][2].grammarSize());
   COL_VAR(Message["PID"][2][0].grammarSize());
   
   EDIparseField(&Message["PID"][2], Field.c_str(), Field.size(), &Here);
}

static void parseEmptyField(){
   COL_FUNCTION(parseEmptyField);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field;
   COL_VAR(Message["PID"][2].grammarSize());
   COL_VAR(Message["PID"][2][0].grammarSize());
   EDIparseField(&Message["PID"][2], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["PID"][2]);
}

static void parseEmptyFields(){
   COL_FUNCTION(parseEmptyFields);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("^");
   COL_VAR(Message["PID"][2].grammarSize());
   COL_VAR(Message["PID"][2][0].grammarSize());
   EDIparseField(&Message["PID"][2], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["PID"][2]);
}

static void parseEmptyFieldsMany(){
   COL_FUNCTION(parseEmptyFieldsMany);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("^^");
   COL_VAR(Message["PID"][2].grammarSize());
   COL_VAR(Message["PID"][2][0].grammarSize());
   EDIparseField(&Message["PID"][2], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["PID"][2]);
   UNIT_ASSERT_EQUALS(0, Message["PID"][2][0].size());
   UNIT_ASSERT_EQUALS(3, Message["PID"][2][0].grammarSize());
}

static void parseMissingField(){
   COL_FUNCTION(parseMissingField);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("foo^");
   COL_VAR(Message["PID"][2].grammarSize());
   COL_VAR(Message["PID"][2][0].grammarSize());
   EDIparseField(&Message["PID"][2], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["PID"][2]);
   UNIT_ASSERT_EQUALS(1, Message["PID"][2][0].size());
   UNIT_ASSERT_EQUALS(3, Message["PID"][2][0].grammarSize());
   UNIT_ASSERT_EQUALS("foo", Message["PID"][2][0][0].value());
   UNIT_ASSERT_EQUALS("", Message["PID"][2][0][1].value());
}

static void parseMissingFieldAndEmptySub(){
   COL_FUNCTION(parseMissingFieldAndEmptySub);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("Smith&&John^Jameson&J&");
   COL_VAR(Message["PID"][2].grammarSize());
   COL_VAR(Message["PID"][2][0].grammarSize());
   EDIparseField(&Message["PID"][2], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["PID"][2]);
   UNIT_ASSERT_EQUALS(3,         Message["PID"][2][0].size());
   UNIT_ASSERT_EQUALS(3,         Message["PID"][2][0].grammarSize());
   UNIT_ASSERT_EQUALS("Smith",   Message["PID"][2][0][0].value());
   UNIT_ASSERT_EQUALS("",        Message["PID"][2][0][1].value());
   UNIT_ASSERT_EQUALS("John",    Message["PID"][2][0][2].value());
   UNIT_ASSERT_EQUALS(3,         Message["PID"][2][1].size());
   UNIT_ASSERT_EQUALS("Jameson", Message["PID"][2][1][0].value());
   UNIT_ASSERT_EQUALS("J",       Message["PID"][2][1][1].value());
}

static void parseMissingSubfieldsAndFields(){
   COL_FUNCTION(parseMissingSubfieldsAndFields);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("Smith&&John^^^Foo"); // Note that this defines more fields than the grammar supports
   COL_VAR(Message["PID"][2].grammarSize());
   COL_VAR(Message["PID"][2][0].grammarSize());
   EDIparseField(&Message["PID"][2], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["PID"][2]);
   UNIT_ASSERT_EQUALS(3,       Message["PID"][2][0].size());
   UNIT_ASSERT_EQUALS(3,       Message["PID"][2][0].grammarSize());
   UNIT_ASSERT_EQUALS("Smith", Message["PID"][2][0][0].value());
   UNIT_ASSERT_EQUALS("",      Message["PID"][2][0][1].value());
   UNIT_ASSERT_EQUALS("John",  Message["PID"][2][0][2].value());
   UNIT_ASSERT_EQUALS(0,       Message["PID"][2][1].size());
   UNIT_ASSERT_EQUALS(3,       Message["PID"][2][0].grammarSize());
   UNIT_ASSERT_EQUALS(0,       Message["PID"][2][2].size());
   UNIT_ASSERT_EQUALS(3,       Message["PID"][2][0].grammarSize());
}

static void parseSubfieldWithFieldSeparator(){
   COL_FUNCTION(parseSubfieldWithFieldSeparator);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("Smith&&^John^");
   COL_VAR(Message["PID"][2].grammarSize());
   COL_VAR(Message["PID"][2][0].grammarSize());
   EDIparseField(&Message["PID"][2], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["PID"][2]);
   UNIT_ASSERT_EQUALS(3,       Message["PID"][2][0].size());
   UNIT_ASSERT_EQUALS(3,       Message["PID"][2][0].grammarSize());
   UNIT_ASSERT_EQUALS("Smith", Message["PID"][2][0][0].value());
   UNIT_ASSERT_EQUALS("",      Message["PID"][2][0][1].value());
   UNIT_ASSERT_EQUALS("",      Message["PID"][2][0][2].value());
   UNIT_ASSERT_EQUALS(1,       Message["PID"][2][1].size());
   UNIT_ASSERT_EQUALS(3,       Message["PID"][2][0].grammarSize());
   UNIT_ASSERT_EQUALS("John",  Message["PID"][2][1][0].value());
   UNIT_ASSERT_EQUALS(0,       Message["PID"][2][2].size());
   UNIT_ASSERT_EQUALS(3,       Message["PID"][2][0].grammarSize());
}

static void parseFieldRepeating(){
   COL_FUNCTION(parseFieldRepeating);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("Muir^Eliot~Big^Bro~Smith^John");
   EDIparseRepeatingField(&Message["PID"][1], Field.c_str(), Field.size(), &Here);
}

static void parseFieldRepeatingSingle(){
   COL_FUNCTION(parseFieldRepeatingSingle);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("ID1^Eliot~ID2^Bro~ID3");
   EDIparseRepeatingField(&Message["PID"][0], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["PID"][0]);
   UNIT_ASSERT_EQUALS("ID1", Message["PID"][0][0].value());
   UNIT_ASSERT_EQUALS("ID2", Message["PID"][0][1].value());
   UNIT_ASSERT_EQUALS("ID3", Message["PID"][0][2].value());
}

static void parseFieldRepeatingMissing(){
   COL_FUNCTION(parseFieldRepeatingMissing);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("ID1^Eliot~ID2^Bro~");
   EDIparseRepeatingField(&Message["PID"][0], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["PID"][0]);
   UNIT_ASSERT_EQUALS("ID1", Message["PID"][0][0].value());
   UNIT_ASSERT_EQUALS("ID2", Message["PID"][0][1].value());
   UNIT_ASSERT_EQUALS("",    Message["PID"][0][2].value());
}

static void parseFieldRepeatingManyMissing(){
   COL_FUNCTION(parseFieldRepeatingManyMissing);
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Field("ID1^Eliot~ID2^Bro~~~IDX");
   EDIparseRepeatingField(&Message["PID"][0], Field.c_str(), Field.size(), &Here);
   COL_VAR(Message["PID"][0]);
   UNIT_ASSERT_EQUALS("ID1", Message["PID"][0][0].value());
   UNIT_ASSERT_EQUALS("ID2", Message["PID"][0][1].value());
   UNIT_ASSERT_EQUALS("",    Message["PID"][0][2].value());
   UNIT_ASSERT_EQUALS("",    Message["PID"][0][3].value());
   UNIT_ASSERT_EQUALS("IDX", Message["PID"][0][4].value());
}

static void parseSegment(){
   COL_FUNCTION(parseSegment);
   COLlist<COLstring> Skipped;
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Segment("PID|ID1^Eliot~ID2^Bro~ID3|Muir^Eliot~Big^Bro~Smith^John|Muir&Eliot|\r");
   COLhashmap <COLstring, bool> MessageSegments;
   MessageSegments.add("PID", true);
   int Consumed = EDIparseSegment(&Message["PID"], Segment.c_str(), Segment.size(), MessageSegments, Skipped, &Here);
   COL_DUMP("Consumed:", Segment.c_str(), ::abs(Consumed));
}

static void parseSegmentZeroFields() {
   COL_FUNCTION(parseSegmentZeroFields);
   COLlist<COLstring> Skipped;
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Segment("PID|\r");
   COLhashmap <COLstring, bool> MessageSegments;
   MessageSegments.add("PID", true);
   int Consumed = EDIparseSegment(&Message["PID"], Segment.c_str(), Segment.size(), MessageSegments, Skipped, &Here);
   COL_TRC_DUMP("Consumed:", Segment.c_str(), ::abs(Consumed));
   UNIT_ASSERT_EQUALS(5, Consumed)
}

static void parseSegmentRepeatField(){
   COL_FUNCTION(parseSegmentRepeatField);
   COLlist<COLstring> Skipped;
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Segment("PID|ID1^Eliot~ID2^Bro~ID3|Muir^Eliot~Big^Bro~Smith^John|Muir&Eliot~Ignore^Me|\r");
   COLhashmap <COLstring, bool> MessageSegments;
   MessageSegments.add("PID", true);
   int Consumed = EDIparseSegment(&Message["PID"], Segment.c_str(), Segment.size(), MessageSegments, Skipped, &Here);
}

static void parseSegmentName(){
   COL_FUNCTION(parseSegmentName);
   COLstring Segment("PID|ID1^Eliot~ID2^Bro~ID3|Muir^Eliot~Big^Bro~Smith^John|Muir&Eliot|");
   COLstring Name;
   EDIparseSegmentName(&Name, Segment.c_str(), Segment.size());
   COL_VAR(Name);
   UNIT_ASSERT_EQUALS(Name, "PID");
}

static void parseSegmentArray(){
   COL_FUNCTION(parseSegmentArray);
   COLlist<COLstring> Skipped;
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Segments("NK1|Muir^Eliot|\rNK1|Muir^Mary|\rNK1|Muir^Fred|\rIN1|dsdfs|\r");
   COLhashmap <COLstring, bool> MessageSegments;
   MessageSegments.add("NK1", true);
   MessageSegments.add("IN1", true);
   int Consumed = EDIparseSegmentCollection(&Message["NK1"], Segments.c_str(), Segments.size(), MessageSegments, Skipped, &Here);
   COL_DUMP("Consumed:", Segments.c_str(), Consumed);
   UNIT_ASSERT_EQUALS(Consumed, 46);
}

static void parseSegmentGroup(){
   COL_FUNCTION(parseSegmentGroup);
   COLlist<COLstring> Skipped;
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Segments("IN1|ID1|\rIN2|ID2|\rPID|fff|\r");
   COLhashmap <COLstring, bool> MessageSegments;
   MessageSegments.add("IN1", true);
   MessageSegments.add("IN2", true);
   MessageSegments.add("PID", true);
   int Consumed = EDIparseSegmentCollection(&Message["INSURANCE"][0], Segments.c_str(), Segments.size(), MessageSegments, Skipped, &Here);
   COL_DUMP("Consumed:", Segments.c_str(), Consumed);
   UNIT_ASSERT_EQUALS("ID1", Message["INSURANCE"][0][0][0].value());
   UNIT_ASSERT_EQUALS("ID2", Message["INSURANCE"][0][1][0].value());
   UNIT_ASSERT_EQUALS(Consumed, 18);
}

static void parseMessage(){
   COL_FUNCTION(parseMessage);
   COLlist<COLstring> Skipped;
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Data("MSH|FF|ADT^A01|\rPID|ID1^Eliot~ID2^Bro~ID3|Muir^Eliot~Big^Bro~Smith^John|Muir&Eliot|\rNK1|Muir^Eliot|\rNK1|Muir^Mary|\rNK1|Muir^Fred|\rIN1|ID1|\rIN2|ID2|\rIN1|ID3|\rIN2|ID4|\r");
   COLhashmap <COLstring, bool> MessageSegments;
   MessageSegments.add("MSH", true);
   MessageSegments.add("PID", true);
   MessageSegments.add("NK1", true);
   MessageSegments.add("IN1", true);
   MessageSegments.add("IN2", true);
   int Consumed = EDIparseSegmentCollection(&Message, Data.c_str(), Data.size(), MessageSegments, Skipped, &Here);
   COL_DUMP("Consumed:", Data.c_str(), Consumed);
   COLstring Result;
   Message.flatwire(&Result);
   COL_DUMP("Flatwire:", Result.c_str(), Result.size());
   EDIdata& Insurance = Message["INSURANCE"][1];
   Result.clear();
   Insurance.flatwire(&Result);
   COL_DUMP("Flatwire:", Result.c_str(), Result.size());
   
   //UNIT_ASSERT_EQUALS(Consumed, 46);*/
}

static void parseMessageZeroSegments() {
   COL_FUNCTION(parseMessageZeroSegments);
   COLlist<COLstring> Skipped;
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createEmptyGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Data("");
   COLhashmap <COLstring, bool> MessageSegments;
   int Consumed = EDIparseSegmentCollection(&Message, Data.c_str(), Data.size(), MessageSegments, Skipped, &Here);
   COL_TRC_DUMP("Consumed:", Data.c_str(), Consumed);
   UNIT_ASSERT_EQUALS(0, Consumed)
}

static void parseMessageNoTrailing(){
   COL_FUNCTION(parseMessageNoTrailing);
   COLlist<COLstring> Skipped;
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Data("MSH|FF|ADT^A01\rPID|ID1^Eliot~ID2^Bro~ID3|Muir^Eliot~Big^Bro~Smith^John|Muir&Eliot\rNK1|Muir^Eliot\rNK1|Muir^Mary|\rNK1|Muir^Fred|\rIN1|ID1|\rIN2|ID2|\rIN1|ID3|\rIN2|ID4|\r");
   COLhashmap <COLstring, bool> MessageSegments;
   MessageSegments.add("MSH", true);
   MessageSegments.add("PID", true);
   MessageSegments.add("NK1", true);
   MessageSegments.add("IN1", true);
   MessageSegments.add("IN2", true);
   int Consumed = EDIparseSegmentCollection(&Message, Data.c_str(), Data.size(), MessageSegments, Skipped, &Here);
   COL_DUMP("Consumed:", Data.c_str(), Consumed);
   COLstring Result;
   Message.flatwire(&Result);
   COL_DUMP("Flatwire:", Result.c_str(), Result.size());
   EDIdata& Insurance = Message["INSURANCE"][1];
   Result.clear();
   Insurance.flatwire(&Result);
   COL_DUMP("Flatwire:", Result.c_str(), Result.size());
   
   //UNIT_ASSERT_EQUALS(Consumed, 46);*/
}

static void parseSegmentComplex(){
   COL_FUNCTION(parseSegmentComplex);
   COLlist<COLstring> Skipped;
   EDIcollection Collection;
   COLarray<COLstring> Here;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Data("MSH|^|^|^|||\r");
   COLhashmap <COLstring, bool> MessageSegments;
   MessageSegments.add("MSH", true);
   int Consumed = EDIparseSegment(&Message["MSH"], Data.c_str(), Data.size(), MessageSegments, Skipped, &Here);
   COL_DUMP("Consumed:", Data.c_str(), ::abs(Consumed));
   COLstring Result;
   Message.flatwire(&Result);
   COL_DUMP("Flatwire:", Result.c_str(), Result.size());
   EDIdata& Insurance = Message["INSURANCE"][1];
   Result.clear();
   Insurance.flatwire(&Result);
   COL_DUMP("Flatwire:", Result.c_str(), Result.size());
   
   //UNIT_ASSERT_EQUALS(Consumed, 46);*/
}

static void parseWithRules() {
   COL_FUNCTION(parseWithRules);
   COLstring Data, ErrorMsg;
   FILreadFile("hl7/msg1.txt", &Data);
   EDIhl7Preprocess(&Data, &ErrorMsg);
   COL_DUMP("Message:", Data.c_str(), Data.size());
   EDIcollection* pCollection = loadCollection("hl7/demo.vmd");
   COLstring Name;
   bool Success = EDIparseMshWithRules(*pCollection, Data, &Name);
   UNIT_ASSERT_EQUALS("ADT", Name);
   UNIT_ASSERT_EQUALS(Success, true);
   delete pCollection;
}

static void matchNoMsh(){
   COL_FUNCTION(matchNoMsh);
   COLstring Name;
   EDIcollection* pCollection = loadCollection("hl7/demo.vmd");
   bool Success = EDIparseMshWithRules(*pCollection, "PID|||", &Name);
   UNIT_ASSERT_EQUALS(Success, false);
   UNIT_ASSERT_EQUALS(Name, "Required MSH segment not found at the start of the message.");
   delete pCollection;
}

static void matchNoMsh9Field(){
   COL_FUNCTION(matchNone);
   COLstring Name;
   EDIcollection* pCollection = loadCollection("hl7/demo.vmd");
   bool Success = EDIparseMshWithRules(*pCollection, "MSH|^~\\&|EPIC|EPICADT|SMS|SMSADT|199912271408|", &Name);
   COL_VAR2(Success, Name);
   UNIT_ASSERT_EQUALS(Success, false);
   UNIT_ASSERT_EQUALS(Name, "Required MSH.9 field not present.");
   delete pCollection;
}

static void matchNone(){
   COL_FUNCTION(matchNone);
   COLstring Name, Data, ErrorMsg;
   FILreadFile("hl7/msg2.txt", &Data);
   EDIhl7Preprocess(&Data, &ErrorMsg);
   EDIcollection* pCollection = loadCollection("hl7/matchNone.vmd");
   bool Success = EDIparseMshWithRules(*pCollection, Data, &Name);
   COL_VAR2(Success, Name);
   UNIT_ASSERT_EQUALS(false, Success);
   UNIT_ASSERT_EQUALS("No matching rules exist that match a message with MSH.9 = 'ZDT^A07'\nRules defined with wildcard expressions to match to messages:\n1. ADT* --> ADT\n", Name);
   delete pCollection;
}

static void parseMsh9() {
   COL_FUNCTION(parseMsh9);
   COLstring Data, ErrorMsg;
   FILreadFile("hl7/msg1.txt", &Data);
   EDIhl7Preprocess(&Data, &ErrorMsg);
   COLstring Actual;
   bool Result = EDIgetMsh9(Data, &Actual);
   UNIT_ASSERT_EQUALS("ADT^A07", Actual);
   UNIT_ASSERT_EQUALS(Result, true);
}

static void testListMessages() {
   COL_FUNCTION(testListMessages);
   EDIcollection Collection;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   COLstring Data("MSH|FF|ADT^A01|\rPID|ID1^Eliot~ID2^Bro~ID3|Muir^Eliot~Big^Bro~Smith^John|Muir&Eliot|\rNK1|Muir^Eliot|\rNK1|Muir^Mary|\rNK1|Muir^Fred|\rIN1|ID1|\rIN2|ID2|\rIN1|ID3|\rIN2|ID4|\r");
   COLhashmap <COLstring, bool> MessageSegments;
   MessageSegments.add("MSH", true);
   MessageSegments.add("PID", true);
   MessageSegments.add("NK1", true);
   MessageSegments.add("IN1", true);
   MessageSegments.add("IN2", true);
   COLstring List;
   EDIlistMessages(Collection, &List);
   COL_VAR(List);
   UNIT_ASSERT_EQUALS("ADTA01 ", List);
}

void testParser(UNITapp* pApp){
   pApp->add("parse/list_messages",            &testListMessages);
   pApp->add("parse/Msh9",                     &parseMsh9);
   pApp->add("parse/rules",                    &parseWithRules);
   pApp->add("parse/subfield",                 &parseSubField);
   pApp->add("parse/subfield/empty",           &parseEmptySubField);
   pApp->add("parse/subfield/single",          &parseSingleSubField);
   pApp->add("parse/subfield/missing",         &parseMissingSubField);
   pApp->add("parse/subfield/multi/missing",   &parseMultipleEmptySubField);
   pApp->add("parse/subfield/over",            &parseMoreSubFieldThanGrammar);
   pApp->add("parse/subfield/field/sep",       &parseSubfieldWithFieldSeparator);
   pApp->add("parse/field",                    &parseField);
   pApp->add("parse/field/single",             &parseSingleField);
   pApp->add("parse/field/empty",              &parseEmptyField);
   pApp->add("parse/field/empty/multiple",     &parseEmptyFields);
   pApp->add("parse/field/empty/multiple/2",   &parseEmptyFieldsMany);
   pApp->add("parse/field/missing",            &parseMissingField);
   pApp->add("parse/field/missing/sub",        &parseMissingFieldAndEmptySub);
   pApp->add("parse/field/missing/empty",      &parseMissingSubfieldsAndFields);
   pApp->add("parse/field/repeating",          &parseFieldRepeating);
   pApp->add("parse/field/repeating/single",   &parseFieldRepeatingSingle);
   pApp->add("parse/field/repeating/missing",  &parseFieldRepeatingMissing);
   pApp->add("parse/field/repeating/empty",    &parseFieldRepeatingManyMissing);
   pApp->add("parse/segment/name",             &parseSegmentName);
   pApp->add("parse/segment",                  &parseSegment);
   pApp->add("parse/segment/zero/field",       &parseSegmentZeroFields);
   pApp->add("parse/segment/repeat/field",     &parseSegmentRepeatField);
   pApp->add("parse/segment/array",            &parseSegmentArray);
   pApp->add("parse/segment/group",            &parseSegmentGroup);
   pApp->add("parse/message",                  &parseMessage);
   pApp->add("parse/message/segment/zero",     &parseMessageZeroSegments);
   pApp->add("parse/message/notrailing",       &parseMessageNoTrailing);
   pApp->add("parse/segment/complex",          &parseSegmentComplex);
   pApp->add("match/missing/msh",              &matchNoMsh);
   pApp->add("match/none",                     &matchNone);
   pApp->add("match/missing/msh9",             &matchNoMsh9Field);
}