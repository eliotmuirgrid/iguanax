// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testFormatter.cpp
//
//  Description
//
//  Author: Eliot Muir 
//  Date:   Wed  5 Oct 2022 20:45:01 EDT
// ---------------------------------------------------------------------------
#include "testJsonLoad.h"

#include <UNIT/UNITapp.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIdata.h>
#include <EDI/EDIformat.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void createNameGrammar(EDIcollection* pGrammar){
   COL_FUNCTION(createNameGrammar);
   pGrammar->Nodes.add("NM", new EDInode("NM", "Name", EDI_COMPOSITE));
   pGrammar->Nodes.add("ST", new EDInode("ST", "String", EDI_COMPOSITE));
   pGrammar->Nodes.add("PAT", new EDInode("PAT", "Patient", EDI_COMPOSITE));
   pGrammar->Nodes.add("MSG", new EDInode("MSG", "MessageType", EDI_COMPOSITE));

   pGrammar->Nodes.add("PID", new EDInode("PID", "Patient IDentification Segment", EDI_SEGMENT));
   pGrammar->Nodes.add("MSH", new EDInode("MSH", "Message Header Segment", EDI_SEGMENT));
   pGrammar->Nodes.add("ADTA01", new EDInode("ADTA01", "Patient admit message", EDI_MESSAGE));
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
   pGrammar->Nodes["MSH"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "Time"));
   pGrammar->Nodes["MSH"]->Children.push_back(EDIchild(pGrammar->Nodes["MSG"], "Type"));
   pGrammar->Nodes["IN1"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "ID number"));
   pGrammar->Nodes["IN2"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "ID number"));
   pGrammar->Nodes["INSURANCE"]->Children.push_back(EDIchild(pGrammar->Nodes["IN1"], "Insurance segment 1"));
   pGrammar->Nodes["INSURANCE"]->Children.push_back(EDIchild(pGrammar->Nodes["IN2"], "Insurance segment 1"));

   pGrammar->Nodes["ADTA01"]->Children.push_back(EDIchild(pGrammar->Nodes["MSH"], "Message header segment"));
   pGrammar->Nodes["ADTA01"]->Children.push_back(EDIchild(pGrammar->Nodes["PID"], "Patient IDentification segment", false, true)); // repeating segment
   pGrammar->Nodes["ADTA01"]->Children.push_back(EDIchild(pGrammar->Nodes["INSURANCE"], "Insurance Group", true, true));   // repeating segment group
}


void formatHl7(){
   COL_FUNCTION(formatHl7);
   EDIcollection Collection;
   createNameGrammar(&Collection);
   COL_TRC("Create data node");
   EDIdata Patient(Collection, "PAT");
   COL_TRC("Create  children");
   Patient[0][0] = "Muir";
   Patient[0][1] = "Eliot";
   Patient[0][2] = "Mervyn";
   Patient[1][0] = "Segall";
   Patient[1][1] = "Steven";
   Patient[2][0] = "Smith&Weston";
   Patient[2][1] = "Anne(&^|~\\)";
      
   COL_TRC("Let's format!");
   COLstring Out;
   Patient.flatwire(&Out);
   //COLcout << Out << newline;
}

// breaking normal pointer convention here.  Different purpose
static void populatePID(EDIdata& PID){
   COL_FUNCTION(populatePID);
   PID[0]       = "0"; // Patient 0 in Contagion!
   COL_TRC("Populated ID");
   PID[1][0][0] = "James";
   COL_TRC("Populated FirstName");
   PID[1][0][1] = "Bond";
   COL_TRC("Populated LastName");
   PID[1][1][0] = "Rowan";
   PID[1][1][1] = "Atkinson";
}

static void formatHl7Segment(){
   COL_FUNCTION(formatHl7Segment);
   EDIcollection Collection;
   createNameGrammar(&Collection);
   EDIdata PID(Collection, "PID");
   populatePID(PID);
   COLstring Out;
   PID.flatwire(&Out);
   // Important to add newline since HL7 segments are terminated by a \r carriage return - other output will overwrite if we
   // don't have the newline.
   //COLcout << Out << newline;
}

static void formatHl7Message(){
   COL_FUNCTION(formatHl7Message);
   EDIcollection Collection;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   EDIdata& MSH = Message["MSH"];
   EDIdata& PID = Message["PID"];
   EDIdata& INSURANCE = Message["INSURANCE"];
   INSURANCE[0]["IN1"][0] = "1";   // INSURANCE[0].IN1
   INSURANCE[0]["IN2"][0] = "1";   // INSURANCE[0].IN1
   INSURANCE[1]["IN1"][0] = "2";   // INSURANCE[0].IN1
   INSURANCE[1]["IN2"][0] = "2";   // INSURANCE[0].IN1
      
   MSH[0] = "20221006";
   MSH[1][0] = "ADT";
   MSH[1][1] = "A01";
   populatePID(PID[0]);
   populatePID(PID[1]);
   COLstring Out;
   Message.flatwire(&Out);
   Out.replace("\r", "\n");
   //COLcout << Out << newline;
   Out.clear();
   INSURANCE.flatwire(&Out);
   Out.replace("\r", "\n");
   //COLcout << Out << newline;
}

static void testTypeConversion(){
   COL_FUNCTION(testTypeConversion);
   EDIcollection Collection;
   createNameGrammar(&Collection);
   EDInode* pMessage = Collection.Nodes["ADTA01"];
   EDIchild& Child = pMessage->Children[0];
   EDIdataType Type = EDImapChildType(EDIN_MESSAGE, Child);
   COLcout << EDIdataTypeAsString(Type) << newline;
   UNIT_ASSERT_EQUALS(Type, EDIN_SEGMENT);

   EDIchild& Child2 = pMessage->Children[2];
   Type = EDImapChildType(EDIN_MESSAGE, Child2);
   COLcout << EDIdataTypeAsString(Type) << newline;
   UNIT_ASSERT_EQUALS(Type, EDIN_SEGMENT_GROUP_ARRAY);
}

static void createSimpleGrammar(EDIcollection* pGrammar){
   COL_FUNCTION(createNameGrammar);
   pGrammar->Nodes.add("ST", new EDInode("ST", "String", EDI_COMPOSITE));
   pGrammar->Nodes.add("MSH", new EDInode("MSH", "Message Header Segment", EDI_SEGMENT));
   pGrammar->Nodes.add("ADTA01", new EDInode("ADTA01", "Patient admit message", EDI_MESSAGE));
   pGrammar->Nodes["ADTA01"]->Children.push_back(EDIchild(pGrammar->Nodes["MSH"], "Message header segment", true, true));
   pGrammar->Nodes["MSH"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "Chars"));
   pGrammar->Nodes["MSH"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "Repeat", false, true));
   
}

static void testSimpleRepeat(){
   COL_FUNCTION(testSimpleRepeat);
   EDIcollection Collection;
   createSimpleGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   EDIdata& MSH = Message["MSH"];
   MSH[0][0] = "ST";
   COL_TRC("About to add repeat 1");
   EDIdata& MSH1 = MSH[0];
   COL_VAR(EDIdataTypeAsString(MSH1.Type));
   EDIdata& Field2 = MSH1[1];
   COL_VAR(EDIdataTypeAsString(Field2.Type));
   Field2[0] = "Repeat1";
   COL_TRC("About to add repeat 2");
   Field2[1] = "Repeat2";

   COL_VAR(EDIdataTypeAsString(Message.Type));
   COL_VAR(EDIdataTypeAsString(MSH.Type));
   COL_VAR(EDIdataTypeAsString(MSH[0].Type));
   COL_VAR(EDIdataTypeAsString(MSH[0][0].Type));
   COL_VAR(EDIdataTypeAsString(MSH[0][1].Type));
   COL_VAR(EDIdataTypeAsString(MSH[0][1][0].Type));
   COL_VAR(EDIdataTypeAsString(MSH[0][1][1].Type));  
}

static void testHl7escape(){
   COL_FUNCTION(testHl7escape);
   EDIcollection Collection;
   createNameGrammar(&Collection);
   EDIdata Message(Collection, "ADTA01");
   EDIdata& MSH = Message["MSH"]; // This isn't the standard MSH but that's ok. Just want to make sure & gets escaped.
   MSH[0] = "20221006";
   MSH[1][0] = "foo&bar";
   COLstring Out;
   EDIescapeData(&Out, MSH[1][0].value());
   COL_VAR(Out);
   UNIT_ASSERT_EQUALS("foo\\T\\bar", Out);
}

void testFormatter(UNITapp* pApp){
   pApp->add("format/hl7", &formatHl7);
   pApp->add("format/hl7/segment",   &formatHl7Segment);
   pApp->add("format/hl7/message",   &formatHl7Message);
   pApp->add("format/type/test",     &testTypeConversion);
   pApp->add("format/simple/repeat", &testSimpleRepeat);
   pApp->add("format/hl7/escape",    &testHl7escape);
}