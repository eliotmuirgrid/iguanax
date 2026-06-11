#ifndef __CMPCRUNDIR_H__
#define __CMPCRUNDIR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCrunDir
//
// Description: Create a run directory for a component
//
// Author: Matthew Sobkowwski
// Date:   Tuesday August 8, 2023
// ---------------------------------------------------------------------------
class COLstring;

bool CMPCrunDir(const COLstring& ComponentId, const COLstring& RelativePath, const COLstring& CommitId, COLstring* pError);
COLstring CMPCgetRunDir(const COLstring& ComponentId, const COLstring& RunComp);

#endif // end of defensive include