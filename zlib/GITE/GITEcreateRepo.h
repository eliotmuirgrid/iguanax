#ifndef __GITE_CREATE_REPO_H__
#define __GITE_CREATE_REPO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITEcreateRepo
//
// Description:
//
// API to create a repo.
//
// Author: Tyler Brown 
// Date:   Wednesday 29 March 2023 - 06:43PM
// ---------------------------------------------------------------------------
struct GSVitem;

bool GITEcreateRepo(const COLstring& Username, const COLstring& Owner, const COLstring& Repository, COLstring* pGitUrl,
						  COLstring* pError);
bool GITEvalidateInputParameters(const COLstring& Owner, const COLstring& Repository, const GSVitem& Item, COLstring* pError); // exposed for unit test

#endif // end of defensive include
