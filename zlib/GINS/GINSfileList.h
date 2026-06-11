#ifndef __GINS_FILE_LIST_H__
#define __GINS_FILE_LIST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSfileList
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Wednesday 11 September 2024 - 08:31AM
// ---------------------------------------------------------------------------

class SCKloop;
class COLwebRequest;
class COLstring;
class COLvar;

bool GINScommitFileList(const COLstring& Dir, const COLstring& CommitId, const COLstring& RelativePath, COLvar* pOut, COLstring* pError);
void GINSfileList(const COLwebRequest& Request, SCKloop* pLoop);

#endif // end of defensive include
