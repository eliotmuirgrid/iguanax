// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCtag
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 20 August 2025 - 12:21PM
// ---------------------------------------------------------------------------

#include <DOC/DOCtag.h>
#include <DIR/DIRpath.h>


#include <COL/COLsplit.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring DOCtagIndexDir(const COLstring& ComponentId){
   return DIRcomponentDevDir(ComponentId) + "index/";
}

COLstring DOCtagDir(const COLstring& Tag, const COLstring& Id){
   COL_FUNCTION(DOCtagDir);
   COLstring Dir(Tag);
   Dir.strip(COLstring::Both, ' ');
   Dir.replace("_", "/");
   Dir = DOCtagIndexDir(Id) + Dir + "/";
   COL_VAR(Dir);
   return Dir;
}
