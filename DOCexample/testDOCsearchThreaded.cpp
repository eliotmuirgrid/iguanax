// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testDOCsearchThreaded
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Tuesday 07 October 2025 - 01:48PM
// ---------------------------------------------------------------------------
#include "testDOCsearchThreaded.h"

#include <DOC/DOCsearchThreaded.h>
#include <DOC/DOCdocument.h>
#include <DOC/DOCserialize.h>
#include <DOC/DOChex.h>

#include <UNIT/UNITapp.h>

#include <FIL/FILutils.h>

#include <COL/COLvarUtils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testFile(COLstring FileName, bool* pKeepGoing, COLvar* pResult, int* pCount){
   COL_FUNCTION(testFile);
   COLstring Content;
   FILreadFile(FileName, &Content);
   DOCdocument Doc;
   DOCfromJson(Content,&Doc);
   COLvar Row;
   Row["id"]      = Doc.Id;
   Row["created"] = DOCtoHex(Doc.Created);
   Row["title"]   = Doc.Title;
   pResult->push_back(Row);
   (*pCount)--;
   if (0 == *pCount){
      *pKeepGoing = false;
   }
}

static void testFindBefore(){
   COL_FUNCTION(testFindBefore);
   //COLstring Result = DOCsearchFindAfterPosition("./find_test", "56");
   //COL_VAR(Result);

   /////////////////////////////////////////////////ELIOT INFO/////////////////////////////////////////////////////
   // FILsetWorkingDir("/Users/eliotmuir/Library/IguanaX/");
   // int Count = 5;
   // DOCsearchDirectionally("00000000663954", "CustomBlan_plEARnSdN8vZck", true, COLnewClosure2(&testFile, &Count)); 

   // Count = 5;
   // DOCsearchDirectionally("00000000663954", "CustomBlan_plEARnSdN8vZck", false, COLnewClosure2(&testFile, &Count)); 
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////STEVE INFO/////////////////////////////////////////////////////
   FILsetWorkingDir("/Users/stephenpiccone/Library/IguanaX");
   COLvar Result; 
   int Count = 10;
   //1998351cc00
   //f2db673d06c7912a9b6bef778311c5ff
   DOCsearchDirectionally("000000", "DOCdatarep_kEuLrpfsDtuPFi", true, "-1", COLnewClosure2(&testFile, &Result, &Count)); 
   COL_VAR(Result);
   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void testDOCsearchThreaded(UNITapp* pApp) {
   //pApp->add("test/find_before", &testFindBefore);
}


