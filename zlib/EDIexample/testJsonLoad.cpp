// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testJsonLoad.cpp
//
//  Description
//
//  Author: Matthew Sobkowski
//  Date:   10/5/22 3:13 PM
// ---------------------------------------------------------------------------
#include "testJsonLoad.h"

#include <UNIT/UNITapp.h>
#include <EDI/EDIgrammar.h>
#include <EDI/EDIload.h>
#include <EDI/EDIsave.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLvar generateChild(const COLstring& Type, const COLstring& Desc, bool Optional, bool Repeats) {
   COL_FUNCTION(generateChild);
   COLvar Child;
   Child[Type] = Desc;
   Child["opt"] = Optional;
   Child["repeats"] = Repeats;
   return Child;
}

static COLvar generateChild(const COLstring& Type, const COLstring& Desc, const COLstring& Optional, const COLstring& Repeats) {
   COL_FUNCTION(generateChild);
   COLvar Child;
   Child[Type] = Desc;
   if(!Optional.is_null()) { Child["opt"] = Optional; }
   if(!Repeats.is_null()) { Child["repeats"] = Repeats; }
   return Child;
}

static void jsonLoad() {
   COL_FUNCTION(jsonLoad);
   EDIcollection Collection;
   COLvar        Json;
   Json["NM"];
   Json["NM"]["description"] = "Name";
   Json["NM"]["type"]        = "composite";
   Json["ST"];
   Json["ST"]["description"] = "String";
   Json["ST"]["type"]        = "composite";
   Json["PID"];
   Json["PID"]["description"] = "Patient Identification Segment";
   Json["PID"]["type"]        = "segment";
   Json["PID"]["children"].setArrayType();
   Json["PID"]["children"].push_back(generateChild("NM", "Patient Name", COL_T(""), COL_T("")));
   Json["PID"]["children"].push_back(generateChild("NM", "Spouse Name",  COL_T("true"),  COL_T("")));
   Json["PID"]["children"].push_back(generateChild("NM", "Children Names", COL_T("true"),  COL_T("true")));
   Json["PID"]["children"].push_back(generateChild("ST", "Allergies", true, true));
   COLstring Error;
   EDIload(Json.json(), &Collection, &Error);
   COLstring Out;
   EDIsave(Collection, &Out, &Error);
   //COLcout << Out;
}

void testJsonLoad(UNITapp* pApp){
   pApp->add("jsonLoad/load", &jsonLoad);
}