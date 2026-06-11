//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCompare
//
// Description:
//
// testCompare - tests for TRNC library
//
// Author: Eliot Muir
// Date:   8th September 2022
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DASH/DASHlistComparison.h>
#include <UNIT/UNITapp.h>
#include <ctype.h>
COL_LOG_MODULE;

void testCompareUnderscore(){
   COL_FUNCTION(testCompareUnderscore);
   COLstring Name1 = "_Apple";
   COLstring Name2 = "Apple";

   int Compare = Name2.icompare(Name1);
   COL_VAR(Compare);
   COL_VAR(toupper('_'));
   COL_VAR(toupper('A'));
   COL_VAR(toupper('a'));
   COL_VAR(toupper('B'));
   COL_VAR(toupper('b'));
   COL_VAR(DASHtoUpperMod('_'));
   COL_VAR(DASHtoUpperMod('A'));
   bool Result1 = DASHicompare(Name1.c_str(), Name2.c_str()) > 0;
   bool Result2 = COLstring::icompare(Name1.c_str(), Name2.c_str()) > 0;
   COL_VAR(DASHicompare(Name1.c_str(), Name2.c_str()));
   COL_VAR(Name1.icompare(Name2));
   UNIT_ASSERT_EQUALS(Result1, !Result2);
}

void testCompare(UNITapp* pApp){
   pApp->add("compare/underscore", &testCompareUnderscore);
}
