#ifndef __GITU_UNCOMMITTED_CHANGES_H__
#define __GITU_UNCOMMITTED_CHANGES_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUuncommittedChanges
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Tuesday 21 March 2023 - 01:22PM
// ---------------------------------------------------------------------------
class COLstring;
class COLvar;
class COLwebRequest;
class SCKloop;

struct CFGmap;

void GITUparseUncommittedOutput(const COLstring& RelativePath, const COLstring& GitStatusOutput, bool Uncommitted, const COLstring& ParentPath, COLvar* pOut);
void GITUuncommittedChanges(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
