// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testGeneral
//
// Description:
//
// Tests for 
//
// Author: Vismay Shah
// Date:   Wednesday 24 April 2024 - 4:57 PM
// ---------------------------------------------------------------------------
#include "testGeneral.h"

#include <COL/COLlog.h>
#include <GITE/GITEcreateRepo.h>
#include <GSV/GSVmap.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testCreateRepoInputs(){
   COLstring Error;
   COLstring Owner          = "foo";
   COLstring Repo           = "bar";
   COLstring OwnerWithSlash = "foo/fizz";
   COLstring RepoWithSlash  = "bar/baz";
	GSVitem Item(true, GIT_bitbucket_cloud);
   UNIT_ASSERT( GITEvalidateInputParameters(Owner, Repo, Item, &Error));
   UNIT_ASSERT(!GITEvalidateInputParameters(OwnerWithSlash, Repo, Item, &Error));
   UNIT_ASSERT(!GITEvalidateInputParameters(Owner, RepoWithSlash, Item, &Error));
   UNIT_ASSERT(!GITEvalidateInputParameters(OwnerWithSlash, RepoWithSlash, Item, &Error));
}

void testGeneral(UNITapp* pApp) {
   pApp->add("create/repo/inputs", &testCreateRepoInputs);
}
