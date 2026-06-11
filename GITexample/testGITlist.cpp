// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testGITUlist
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir
// Date:   Friday 31 March 2023 - 10:52AM
// ---------------------------------------------------------------------------
#include "testGITlist.h"

#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <GITU/GITUlist.h>
#include <GITU/GITUlistLoad.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;


static void checkList(const GITUlist& List){
   COL_FUNCTION(checkList);
   UNIT_ASSERT_EQUALS(7, List.List.size());
   // Notice we get the name from an LLP Client Library from collection.json but not from .gitmodules
   // UNIT_ASSERT_EQUALS("LLP Client Library", List.List["https://bitbucket.org/interfaceware/llpc.git"].Name);
   // UNIT_ASSERT_EQUALS(GIT_LIBRARY         , List.List["https://bitbucket.org/interfaceware/llpc.git"].RepoType);
   // UNIT_ASSERT_EQUALS("HL7 Sender"        , List.List["https://bitbucket.org/interfaceware/hl7_client.git"].Name);
   // UNIT_ASSERT_EQUALS(GIT_COMPONENT       , List.List["https://bitbucket.org/interfaceware/hl7_client.git"].RepoType);
}

static void testCollectionLoad(){
   COL_FUNCTION(testCollectionLoad);
   GITUlist List;
	COLstring Error;
   GITUlistCollectionLoad("git@bitbucket.org:interfaceware/hl7components.git", "collection.json", &List, &Error);
   COL_VAR(List);
   checkList(List);
}

static void testSubmoduleLoad(){
   COL_FUNCTION(testSubmoduleLoad);
   GITUlist List;
   GITUlistGitModuleLoad("git@bitbucket.org:interfaceware/hl7_viewer.git", ".gitmodules",&List);
   COL_VAR(List);
   // UNIT_ASSERT_EQUALS(2, List.List.size());
   // UNIT_ASSERT_EQUALS("CHK"      , List.List["https://bitbucket.org/interfaceware/chk.git"].Name);
   // UNIT_ASSERT_EQUALS(GIT_LIBRARY, List.List["https://bitbucket.org/interfaceware/chk.git"].RepoType);
   // UNIT_ASSERT_EQUALS("LLPC"     , List.List["https://bitbucket.org/interfaceware/llpc.git"].Name);
   // UNIT_ASSERT_EQUALS(GIT_LIBRARY, List.List["https://bitbucket.org/interfaceware/llpc.git"].RepoType);
}

static void testLoadFirst(){
   COL_FUNCTION(testLoadFirst);
   GITUlist List;
   UNIT_ASSERT(GITUlistLoad(FILworkingDir(), "git@bitbucket.org:interfaceware/hl7components.git", &List));
   COL_VAR(List);
   checkList(List);
}

// TODO - fix the unit test to use the vector instead of the map
void testGITUlist(UNITapp* pApp) {
   // pApp->add("list/collection/load", &testCollectionLoad);
   // pApp->add("list/gitmodules/load", &testSubmoduleLoad);
   // pApp->add("list/load/first",      &testLoadFirst);

}