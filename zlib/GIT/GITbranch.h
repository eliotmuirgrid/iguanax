#ifndef __GIT_BRANCH_H__
#define __GIT_BRANCH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITbranch
//
// Description:
//
// Returns the branches in a repository
//
// Author: Tyler Brown 
// Date:   Tuesday 28 February 2023 - 12:40PM
// ---------------------------------------------------------------------------
// class COLstring;
#include <COL/COLvarUtils.h>

void GITupdateDefaultBranch(const COLstring& Dir);

COLstring GITrepositoryDefaultBranch(const COLstring& Dir);  // Returns name of default branch - i.e. "main" or "master"
COLstring GITbranchCurrent(const COLstring& Dir);  // Returns name of current branch - i.e. "branch_name"
COLstring GITbranchCurrentDev(const COLstring& Dir);

bool GITbranchValidateName(const COLstring& Dir, const COLstring& BranchName, COLstring* pError);
bool GITbranchExistsLocal(const COLstring& Dir, const COLstring& BranchName, const COLstring& Username, bool* pExists, COLstring* pError);
bool GITbranchCreateAndCheckout(const COLstring& Dir, const COLstring& BranchName, COLstring* pError);
bool GITbranchCheckoutExistingOrTrack(const COLstring& Dir, const COLstring& BranchName, COLstring* pError);
bool GITsetLocalDefaultBranch(const COLstring& Dir, const COLstring& Branch, COLstring* pOut, COLstring* pError);

#endif // end of defensive include
