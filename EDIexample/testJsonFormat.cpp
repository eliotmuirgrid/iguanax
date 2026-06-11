//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testJsonFormat
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue  4 Oct 2022 15:16:34 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <EDI/EDIgrammar.h>
#include <EDI/EDIsave.h>


#include <COL/COLlog.h>
COL_LOG_MODULE;


void createTestGrammar(EDIcollection* pGrammar){
   COL_FUNCTION(createTestGrammar);
   pGrammar->Nodes.add("NM", new EDInode("NM", "Name", EDI_COMPOSITE));
   pGrammar->Nodes.add("ST", new EDInode("ST", "String", EDI_COMPOSITE));
   pGrammar->Nodes.add("CMP", new EDInode("CMP", "String", EDI_COMPOSITE));

   pGrammar->Nodes.add("PID", new EDInode("PID", "Patient Identification Segment", EDI_SEGMENT));
   //pGrammar->Nodes.add("\"AWKWARD\"", new EDInode("An \"Awkward String\"", EDI_SEGMENT));

   pGrammar->Nodes["PID"]->Children.push_back(EDIchild(pGrammar->Nodes["NM"], "Patient Name", 1, 1));
   pGrammar->Nodes["PID"]->Children.push_back(EDIchild(pGrammar->Nodes["NM"], "Spouse Name", 0, 1));
   pGrammar->Nodes["PID"]->Children.push_back(EDIchild(pGrammar->Nodes["NM"], "Children Names", 0, -1));
   pGrammar->Nodes["PID"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "Allergies", 2, 10));
   pGrammar->Nodes["PID"]->Children.push_back(EDIchild(pGrammar->Nodes["ST"], "My \"Life work\"", 2, 10));
   pGrammar->Nodes["PID"]->Children.push_back(EDIchild(pGrammar->Nodes["CMP"], "A longer data type", 2, 10));
   //pGrammar->Nodes["PID"]->Children.push_back(EDIchild("\"AWKWARD\"", "My \"Awkward work\"", 2, 10));
}

static void testPretty(){
   COL_FUNCTION(testPretty);
   EDIcollection Grammar;
   createTestGrammar(&Grammar);
   COLstring Out, Err;
   EDIsave(Grammar, &Out, &Err);
   //COLcout << Out;
}

void testJsonFormat(UNITapp* pApp){
   pApp->add("json/simple", &testPretty);
}
