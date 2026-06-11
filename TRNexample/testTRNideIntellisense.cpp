// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTRNideIntellisense
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir 
// Date:   Tuesday 02 May 2023 - 10:14AM
// ---------------------------------------------------------------------------
#include "testTRNideIntellisense.h"

#include <TRN/TRNintellisenseApi.h>
#include <TRN/TRNannotationJob.h>

#include <LUA/LUAutils.h>

#include <UNIT/UNITapp.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <NODL/NODLenvironment.h>

#include <LXML/LXMLapi.h>
#include <LND/LNDapi.h>
#include <LND/LNDutils.h>
#include <LDB/LDBapi.h>
#include <LJSON/LJSONapi.h>
#include <LHLP/LHLPindex.h>

#include <HELP/HELPenumerate.h>

#include <LUA/LUAopen.h>

#include <LHL7/LHL7api.h>

#include <COL/COLsplit.h>
#include <COL/COLclosure.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static size_t testFindCursor(const COLarray<COLstring> Lines, int* pLineNumber){
   COL_FUNCTION(testFileCursor);
   size_t Cursor=npos;
   *pLineNumber=0;
   for (int i =0; i < Lines.size(); i++){
      Cursor = Lines[i].find("😊");
      if (Cursor != npos){
         COL_TRC("Found Cursor");
         *pLineNumber = i+1;
         return Cursor;
      }
   }
   return npos;
}

static void testRunTheCode(const COLstring& Code, int LineNumber, int Cursor, COLvar* pResult){
   COL_FUNCTION(testRunTheCode);
   COLvar P;
   P["component"] = "foo_guid";
   P["filename"] = "main.lua";
   P["sample_data_index"] = 2;
   P["code"] = Code;
   P["line_number"] = LineNumber;
   P["cursor_position"] = Cursor;
   COL_VAR(P.json(1));
   TRNannotationJob State;

   TRNintellisenseApi(P, &State, pResult);

   COL_VAR(pResult->json(1));
}

static void testCheckResult(const COLstring& RecipeFile, const COLvar& Result){
   COL_FUNCTION(testCheckResult);
   COLstring ExpectedOut = FILpathName(RecipeFile);
   ExpectedOut = ExpectedOut.substr(0, ExpectedOut.size()-3) + "json";
   ExpectedOut = FILpathDir(RecipeFile) + "zout." + ExpectedOut;
   COL_VAR(ExpectedOut);
   if (!FILfileExists(ExpectedOut)){
      FILwriteFile(ExpectedOut, Result.json(1));
   } else {
      COLstring Content;
      FILreadFile(ExpectedOut, &Content);
      UNIT_ASSERT_EQUALS(Content,Result.json(1));
   }
}

static void testAutoComplete(COLstring RecipeFile){
   COL_FUNCTION(testCase);
   COL_VAR(RecipeFile);
   COLstring Content;
   FILreadFile(RecipeFile, &Content);
   COL_DUMP("Content:", Content.c_str(), Content.size());
   COLarray<COLstring> Lines;
   COLsplit(&Lines, Content, "\n");
   int LineNumber=0;
   size_t Cursor = testFindCursor(Lines, &LineNumber);
   COL_VAR2(LineNumber, Cursor);  // I think this is silly - we should just deal with the entire buffer.
   UNIT_ASSERT_MESSAGE(Cursor!=npos, "Couldn't find 😊 cursor.");   
   Content.replace("😊", "");
   COLvar Result;
   testRunTheCode(Content, LineNumber, Cursor, &Result);
   testCheckResult(RecipeFile, Result);
}

static void testInhaleTestCases(UNITapp* pApp, COLstring Path){
   COL_FUNCTION(testInhaleTestCases); 
   COLmap<COLstring, bool> FileList;
   FILdirList(&FileList, Path+"/*");
   for (auto i= FileList.begin(); i!= FileList.end(); i++){
      COL_VAR2(i->first, i->second);
      COLstring SubPath = Path + "/" + i->first;
      if (i->second) {
         COL_TRC(SubPath << " is a directory.");
         testInhaleTestCases(pApp, SubPath);
      } else {
         if (SubPath.substr(SubPath.size()-3) == "lua"){
            pApp->addClosure(SubPath.substr(0, SubPath.size()-4), COLnewClosure0(&testAutoComplete, SubPath));
         }
      }
   }
}



void testRegister(lua_State* L, LUAmode Mode, DBdatabaseFactory* pDatabaseFactory, COLhashmap<COLstring, COLstring>* pHelpMap){
   COL_FUNCTION(testRegister);
   LXMLaddXmlNamespace(L);
   LNDaddNodeNamespace(L);
   LNDdefineTree(L);
   LDBaddDatabaseNamespace(L);  // a little non standard
   LJSONapiAddJsonNamespace(L);  // WITHOUT THIS the assignment code in Iguana doesn't work
   LHL7api(L);
   LHLPcreateHelpTable(L);
   LHLPluaHelpIndex(*pHelpMap, L);
   LUAsetPointerField(L, NODL_DATABASE_FACTORY.c_str(), pDatabaseFactory);
}

void loadHelp(COLhashmap<COLstring, COLstring>* pHelpMap){
   COL_FUNCTION(loadHelp);
   HELPenumerateHelp(pHelpMap, "../Iguana/app/help/", "");
}


void testTRNideIntellisense(UNITapp* pApp) {
   testInhaleTestCases(pApp, "tests");
}
