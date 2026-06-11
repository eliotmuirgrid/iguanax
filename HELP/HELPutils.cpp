//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HELPobject
//
// Description:
//
// Implementation
//
// Author: Orkhan Amikishiyev
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <HELP/HELPmodel.h>
#include <HELP/HELPserialize.h>
#include <HELP/HELPutils.h>
COL_LOG_MODULE;

void HELPcontentToVar(const COLstring& Content, COLvar* pData) {
   COL_FUNCTION(HELPcontentToVar);
   HELPobject Help;
   HELPtoObject(&Help, Content);
   HELPtoVar(pData, Help);
}

bool HELPinvalidComponent(const COLstring& Component){
   COL_FUNCTION(HELPinvalidComponent);
   COL_VAR(Component);
   return !FILfileExists(DIRcompsDir() + Component + FIL_DIR_SEPARATOR);
}

bool HELPcheckPathOutOfBounds(const COLstring& Path, const COLstring& Component){
   COL_FUNCTION(HELPcheckPathOutOfBounds);
   // Given an absolute path, make sure it's within the working dir or the executable dir
	const COLstring ComponentDevDir = DIRcomponentDir(Component);
   const COLstring ComponentRunDir = DIRcomponentRunDir(Component);
	const COLstring GlobalHelpDir	  = HELPglobalDir();
	if (Path.find(ComponentDevDir) == npos && Path.find(ComponentRunDir) == npos){
      if (Path.find(GlobalHelpDir) == npos){ return true; }
   }
   return false;
}

COLstring HELPglobalDir(){
   COLstring GlobalHelpDir = FILpathAppend(FILexecutableDir(), "..");
   GlobalHelpDir           = FILpathAppend(GlobalHelpDir, COL_T("help") + FIL_DIR_SEPARATOR);
   return GlobalHelpDir;
}