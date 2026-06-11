//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testFilter.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   30/01/25 10:27 PM
//  ---------------------------------------------------------------------------
#include "testFilter.h"

#include <SRCH/SRCHutils.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testNullFilter() {
   COLvar Filter = COLvar();;
   COLvar Arr1;
   COLvar pos1;
   pos1["file"] = "main.lua";
   pos1["line_number"] = 5;
   pos1["position"] = 8;
   Arr1.push_back(pos1);
   SRCHfilterMatches(Filter, &Arr1);
   COL_TRC(Arr1.json(1));
}

static void testRealCase() {
   COLvar Arr1, Arr2;
   Arr1.setArrayType();
   Arr2.setArrayType();
   
   // Create Arr1 with simple positions
   COLvar pos1, pos2;
   pos1["file"] = "main.lua";
   pos1["line_number"] = 5;
   pos1["position"] = 8;
   
   pos2["file"] = "main.lua";
   pos2["line_number"] = 5;
   pos2["position"] = 19;
   
   Arr1.push_back(pos1);
   Arr1.push_back(pos2);
   
   // Create Arr2 with highlighted HTML
   COLvar match1, match2, match3;
   match1["file"] = "main.lua";
   match1["line"] = "-- &lt;<span style=\"background-color:#FFFBB9;\">div</span>&gt;hello&lt;/div&gt;";
   match1["line_number"] = 5;
   match1["position"] = 8;
   
   match2["file"] = "main.lua";
   match2["line"] = "-- &lt;div&gt;hello&lt;/<span style=\"background-color:#FFFBB9;\">div</span>&gt;";
   match2["line_number"] = 5;
   match2["position"] = 19;
   
   match3["file"] = "main.lua";
   match3["line"] = "iguana.log(&quot;http:&lt;/<span style=\"background-color:#FFFBB9;\">div</span>&gt;&quot;)";
   match3["line_number"] = 7;
   match3["position"] = 23;
   
   Arr2.push_back(match1);
   Arr2.push_back(match2);
   Arr2.push_back(match3);
   
   SRCHfilterMatches(Arr1, &Arr2);
   COL_VAR(Arr2.json(1));
   UNIT_ASSERT(Arr2.size() == 2);
}

static void testSimple() {
   COLvar Arr1, Arr2;
   Arr1.setArrayType();
   Arr2.setArrayType();
   
   COLvar One;
   One["file"] = "1";
   One["line_number"] = 1;
   One["position"] = 2;
   
   // Add elements to first array
   Arr1.push_back(One);
   Arr1.push_back("a");
   Arr1.push_back("b");
   
   // Add elements to second array
   Arr2.push_back(One);
   Arr2.push_back("c");
   Arr2.push_back("d");
   SRCHfilterMatches(Arr1, &Arr2);
   COL_VAR(Arr2.json(1));
   UNIT_ASSERT(Arr2.size() == 1);
}

static void testEmptyArrays() {
   COLvar Arr1, Arr2;
   Arr1.setArrayType();
   Arr2.setArrayType();
   
   SRCHfilterMatches(Arr1, &Arr2);
   UNIT_ASSERT(Arr2.size() == 0);
}

static void testSingleElement() {
   COLvar Arr1, Arr2;
   Arr1.setArrayType();
   Arr2.setArrayType();
   
   COLvar elem;
   elem["file"] = "test.lua";
   elem["line_number"] = 1;
   elem["position"] = 1;
   
   Arr1.push_back(elem);
   Arr2.push_back(elem);
   
   SRCHfilterMatches(Arr1, &Arr2);
   UNIT_ASSERT(Arr2.size() == 1);
}

static void testNoMatches() {
   COLvar Arr1, Arr2;
   Arr1.setArrayType();
   Arr2.setArrayType();
   
   COLvar elem1, elem2;
   elem1["file"] = "test1.lua";
   elem1["line_number"] = 1;
   elem1["position"] = 1;
   
   elem2["file"] = "test2.lua";
   elem2["line_number"] = 2;
   elem2["position"] = 2;
   
   Arr1.push_back(elem1);
   Arr2.push_back(elem2);
   
   SRCHfilterMatches(Arr1, &Arr2);
   UNIT_ASSERT(Arr2.size() == 0);
}

static void testDuplicateElements() {
   COLvar Arr1, Arr2;
   Arr1.setArrayType();
   Arr2.setArrayType();
   
   COLvar elem;
   elem["file"] = "test.lua";
   elem["line_number"] = 1;
   elem["position"] = 1;
   
   Arr1.push_back(elem);
   Arr2.push_back(elem);
   Arr2.push_back(elem);
   
   SRCHfilterMatches(Arr1, &Arr2);
   COL_VAR(Arr2.json(1));
   UNIT_ASSERT(Arr2.size() == 1);
}

static void testDifferentFiles() {
   COLvar Arr1, Arr2;
   Arr1.setArrayType();
   Arr2.setArrayType();
   
   COLvar elem1, elem2;
   elem1["file"] = "test1.lua";
   elem1["line_number"] = 1;
   elem1["position"] = 1;
   
   elem2["file"] = "test2.lua";
   elem2["line_number"] = 1;
   elem2["position"] = 1;
   
   Arr1.push_back(elem1);
   Arr2.push_back(elem2);
   
   SRCHfilterMatches(Arr1, &Arr2);
   UNIT_ASSERT(Arr2.size() == 0);
}

void testFilter(UNITapp* pApp){
   COL_FUNCTION(testFilter);
   pApp->add("filter/simple", testSimple);
   pApp->add("filter/realCase", testRealCase);
   pApp->add("filter/empty", testEmptyArrays);
   pApp->add("filter/single", testSingleElement);
   pApp->add("filter/noMatches", testNoMatches);
   pApp->add("filter/duplicates", testDuplicateElements);
   pApp->add("filter/differentFiles", testDifferentFiles);
   pApp->add("filter/none", testNullFilter);
}