//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testApi
//
// Description:
//
// Implementation
//
// Author: Orkhan
// Date:   8th Sept 2022
//---------------------------------------------------------------------------
#include "testApi.h"

#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmap.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLlog.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PASS/PASSword.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testCFGresolveRepoFileValid(){
   COL_FUNCTION(testResolveComponent);
	PASSsetKeyIV("test", "test");
   COLstring Path = CFGresolveRepoFile("test123", "HTTP/HTTPget.help", true);
   COL_VAR(Path);
   UNIT_ASSERT_EQUALS(FILpathAppend(FILworkingDir(), "config/comps/test123/dev/HTTP/HTTPget.help"), Path);
}

static void testCFGresolveRepoFileInvalid(){
   COL_FUNCTION(testCFGresolveRepoFileInvalid);
   COLstring RelativePath = ".." + COL_T(FIL_DIR_SEPARATOR) + ".." + COL_T(FIL_DIR_SEPARATOR) + "password";
   COLstring Path = CFGresolveRepoFile("test_Ticker2323", "../../password", true);
   COL_VAR(Path);
   UNIT_ASSERT_EQUALS("", Path);
}

static void testCFGresolveRepoFileInvalid2(){
   COL_FUNCTION(testCFGresolveRepoFileInvalid2);
   COLstring Path = CFGresolveRepoFile("test_Ticker2323", "..", true);
   COL_VAR(Path);
   UNIT_ASSERT_EQUALS("", Path);
}

static void testCFGresolveRepoFileInvalid3(){
   COL_FUNCTION(testCFGresolveRepoFileInvalid3);
   COLstring Path1 = CFGresolveRepoFile("////", "foo.txt", true);
   COLstring Path2 = CFGresolveRepoFile("\\\\\\\\", "foo.txt", true);
   COL_VAR2(Path1, Path2);
   UNIT_ASSERT_EQUALS("", Path1);
   UNIT_ASSERT_EQUALS("", Path2);
}

static void testCFGresolveRepoFileInvalid4(){
   COL_FUNCTION(testCFGresolveRepoFileInvalid4);
   COLstring Path1 = CFGresolveRepoFile("//..//.", "foo.txt", true);
   COLstring Path2 = CFGresolveRepoFile("\\\\..\\\\.", "foo.txt", true);
   COL_VAR2(Path1, Path2);
   UNIT_ASSERT_EQUALS("", Path1);
   UNIT_ASSERT_EQUALS("", Path2);
}

static void testComponentMapCycle(){
   COL_FUNCTION(testComponentMapCycle);
   CFGmap ConfigMap;

   CFGconfig ComponentA;
   ComponentA.CoreConfig.Guid = "ComponentA";
   ComponentA.CoreConfig.Sources.push_back("ComponentB");
   CFGconfig ComponentB;
   ComponentB.CoreConfig.Guid = "ComponentB";
   ComponentB.CoreConfig.Sources.push_back("ComponentC");
   CFGconfig ComponentC;
   ComponentC.CoreConfig.Guid = "ComponentC";
   ComponentC.CoreConfig.Sources.push_back("ComponentA");
   CFGmapAddComponent(&ConfigMap, ComponentA);
   CFGmapAddComponent(&ConfigMap, ComponentB);
   CFGmapAddComponent(&ConfigMap, ComponentC);
   UNIT_ASSERT( CFGmapHasComponentCycle(&ConfigMap, "ComponentA"));
   UNIT_ASSERT( CFGmapHasComponentCycle(&ConfigMap, "ComponentB"));
   UNIT_ASSERT( CFGmapHasComponentCycle(&ConfigMap, "ComponentC"));

   CFGconfig ComponentD;
   ComponentD.CoreConfig.Guid = "ComponentD";
   ComponentD.CoreConfig.Sources.push_back("ComponentA");
   CFGmapAddComponent(&ConfigMap, ComponentD);
   UNIT_ASSERT( CFGmapHasComponentCycle(&ConfigMap, "ComponentD"));

   CFGconfig ComponentE;
   ComponentE.CoreConfig.Guid = "ComponentE";
   ComponentE.CoreConfig.Sources.push_back("ComponentF");
   CFGconfig ComponentF;
   ComponentF.CoreConfig.Guid = "ComponentF";
   CFGmapAddComponent(&ConfigMap, ComponentE);
   CFGmapAddComponent(&ConfigMap, ComponentF);
   UNIT_ASSERT(!CFGmapHasComponentCycle(&ConfigMap, "ComponentE"));
   UNIT_ASSERT(!CFGmapHasComponentCycle(&ConfigMap, "ComponentF"));

   CFGconfig ComponentG;
   ComponentG.CoreConfig.Guid = "ComponentG";
   CFGmapAddComponent(&ConfigMap, ComponentG);
   UNIT_ASSERT(!CFGmapHasComponentCycle(&ConfigMap, "ComponentG"));
}

void testApi(UNITapp* pApp){
   pApp->add("component/CFGresolveRepoFile/good", &testCFGresolveRepoFileValid);
   pApp->add("component/CFGresolveRepoFile/bad", &testCFGresolveRepoFileInvalid);
   pApp->add("component/CFGresolveRepoFile/bad2", &testCFGresolveRepoFileInvalid2);
   pApp->add("component/map/cycle", &testComponentMapCycle);
   // pApp->add("component/CFGresolveRepoFile/bad3", &testCFGresolveRepoFileInvalid3);
   // pApp->add("component/CFGresolveRepoFile/bad4", &testCFGresolveRepoFileInvalid4); TODO - get this to work with the GIT paths
}
