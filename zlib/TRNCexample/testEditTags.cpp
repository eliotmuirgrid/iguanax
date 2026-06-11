//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testEditTags.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   14/11/23 3:36 PM
//  ---------------------------------------------------------------------------
#include "testEditTags.h"

#include <TRNC/TRNCeditTags.h>

#include <UNIT/UNITapp.h>

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static COLarray<COLstring> setupDescription() {
   COLarray<COLstring> Description;
   Description.push_back("#partial");
   Description.push_back("#dev");
   Description.push_back("#prod");
   return Description;
}

static void testPartial() {
   COL_FUNCTION(testPartial);
   COLarray<COLstring> Description = setupDescription();
   COLarray<COLstring> AddVec;
   COLmap<COLstring, bool> Lookup;
   Lookup.add("#partial", true);
   TRNCeditTagsUpdate(&Description, AddVec, Lookup);
   COLASSERT(Description.size() == 2);
}

static void testClear() {
   COL_FUNCTION(testClear);
   COLarray<COLstring> Description = setupDescription();
   COLarray<COLstring> AddVec;
   COLmap<COLstring, bool> Lookup;
   Lookup.add("#partial", true);
   Lookup.add("#dev", true);
   Lookup.add("#prod", true);
   TRNCeditTagsUpdate(&Description, AddVec, Lookup);
   COLASSERT(Description.size() == 0);
}

static void testMultiple() {
   COL_FUNCTION(testMultiple);
   COLarray<COLstring> Description = setupDescription();
   COLarray<COLstring> AddVec;
   AddVec.push_back("#setup");
   COLmap<COLstring, bool> Lookup;
   Lookup.add("#partial", true);
   Lookup.add("#dev", true);
   Lookup.add("#prod", true);
   TRNCeditTagsUpdate(&Description, AddVec, Lookup);
   COLASSERT(Description.size() == 1);
   COLASSERT(Description[0] == "#setup");
}

static void testSimple() {
   COL_FUNCTION(testSimple);
   COLarray<COLstring> Description;
   COLarray<COLstring> AddVec;
   AddVec.push_back("#test");
   COLmap<COLstring, bool> Lookup;
   Lookup.add("#remove", true);
   TRNCeditTagsUpdate(&Description, AddVec, Lookup);
   COLASSERT(Description.size() == 1);
   COLASSERT(Description[0] == "#test");
}

void testEditTags(UNITapp* pApp) {
   pApp->add("edit/simple",   &testSimple);
   pApp->add("edit/multiple", &testMultiple);
   pApp->add("edit/clear",    &testClear);
   pApp->add("edit/partial",  &testPartial);
}