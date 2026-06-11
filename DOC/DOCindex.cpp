// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCindex
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 21 August 2025 - 08:49AM
// ---------------------------------------------------------------------------

#include <DOC/DOCindex.h>
#include <DOC/DOCdocument.h>
#include <DOC/DOCtag.h>
#include <DOC/DOCserialize.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void DOCindexTag(const DOCdocument& Doc, const COLstring& Tag, const COLstring& Id){
   COL_FUNCTION(DOCindexTag);
   // write out the tag file for one tag.
   COL_VAR(Tag);   
   COLstring Dir = DOCtagDir(Tag, Id);
   COL_VAR(Dir);
   COLstring FileName = Dir + Doc.Id;
   COL_VAR2(Dir, FileName);
   FILmakeFullDir(Dir, FIL_USER_RWX, NULL);
   COLstring Content;
   FILwriteFile(FileName+".md",Doc.Data);
   DOCtoJson(Doc, &Content);
   FILwriteFile(FileName+".info", Content);
   
}

void DOCindex(const DOCdocument& Doc, const COLstring& IndexDir){
   COL_FUNCTION(DOCindex);
   for (auto i = Doc.Tags.cbegin(); i != Doc.Tags.cend(); i++){
      DOCindexTag(Doc, i->first, IndexDir);
   }
}
