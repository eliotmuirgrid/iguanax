// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PACKattachC
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 16 August 2023 - 12:09PM
// ---------------------------------------------------------------------------

#include <PACK/PACKbinToC.h>
#include <PACK/PACKattachC.h>

#include <FIL/FILutils.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void PACKattachC(const COLvar& Result, COLstring CFileName){
   COL_FUNCTION(PACKattachC);
   COLstring Attachment;
   Result.toBinary(&Attachment);
   COLstring FileContent;
   PACKbinToC(Attachment, &FileContent);
   FILwriteFile(CFileName, FileContent);
   COLcout << FILexecutableDir();
}
