//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testPath
//
// Description:
//
// Implmentation
//
// Author: Eliot Muir
// Date:   Thu 23 Mar 2023 15:22:04 EDT
//---------------------------------------------------------------------------
#include "testPath.h"

#include <COL/COLlog.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITpath.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testTemplateDir(){
   COL_FUNCTION(testTemplateDir);
   COLstring SourceDir = GITgitTemplateDir("git@bitbucket.org:interfaceware/interfaceware-core.git");
   COLstring Expected = FILpathAppend(FILworkingDir(),"git_cache/bitbucket.org/interfaceware/interfaceware-core/");
   FILcorrectPathSeparators(Expected);
   COL_VAR2(SourceDir, Expected);
   UNIT_ASSERT_EQUALS(Expected, SourceDir);
}

void testPath(UNITapp* pApp){
   COL_FUNCTION(testPath);
   pApp->add("path/templatedir",&testTemplateDir);
}
