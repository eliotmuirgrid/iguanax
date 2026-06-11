#ifndef __GINS_DIFF_H__
#define __GINS_DIFF_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSdiff
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Wednesday 11 September 2024 - 07:55AM
// ---------------------------------------------------------------------------
class SCKloop;
class COLwebRequest;

void GINSgetFileDiffs(COLstring Path, COLstring CommitId, COLstring FileName, COLvar* pOutput);
void GINSdiff(const COLwebRequest& Request, SCKloop* pLoop);


#endif // end of defensive include
