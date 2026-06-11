//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testFSBextractPath
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sun 21 Mar 2021 22:08:12 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include <FSB/FSBextractPath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testExtractPathRoot(){
   COL_FUNCTION(testExtractPathRoot);
   COLvar Out;
   FSBfileBrowserExtractPathImp(Out, "~/");
   COL_VAR(Out);
   COLstring RootDir = FILexpand("~/");
   RootDir = FILpathSepToNative(RootDir);
   COL_VAR(RootDir);
   UNIT_ASSERT_EQUALS(RootDir, Out["path"]);
}

static void testExtractPathInvalid(){
   COL_FUNCTION(testExtractPathInvalid);
   COLvar Out;
   FSBfileBrowserExtractPathImp(Out, "/bad_path/bad/");// completely invalid path, should resolve to root dir
   COL_VAR(Out);
   COLstring RootDir = FILexpand("~/");
   RootDir = FILpathSepToNative(RootDir);
   COL_VAR(RootDir);
   UNIT_ASSERT_EQUALS(RootDir, Out["path"]);
}

static void testExtractPathFolderNotExist(){
   COL_FUNCTION(testExtractPathFolderNotExist);
   COLvar Out;
   FSBfileBrowserExtractPathImp(Out, "./not_exist/"); // should resolve to first valid parent dir, which is this FSBexample folder
   COL_VAR(Out);
   COLstring CurrentDir = FILworkingDir();
   COL_VAR(CurrentDir);
   UNIT_ASSERT_EQUALS(CurrentDir, Out["path"]);
}

static void testExtractPathParentDir(){
   COL_FUNCTION(testExtractPathParentDir);
   COLvar Out;
   FSBfileBrowserExtractPathImp(Out, "./testFSBextractPath.cpp"); // should resolve to file's parent dir, which is this FSBexample folder
   COL_VAR(Out);
   COLstring CurrentDir = FILworkingDir();
   COL_VAR(CurrentDir);
   UNIT_ASSERT_EQUALS(CurrentDir, Out["path"]);
}

void testFSBextractPath(UNITapp* pApp){
   pApp->add("extract_path/root", &testExtractPathRoot);
   pApp->add("extract_path/invalidPath", &testExtractPathInvalid);
   pApp->add("extract_path/folderNotExist", &testExtractPathFolderNotExist);
   pApp->add("extract_path/fileParentDir", &testExtractPathParentDir);
}