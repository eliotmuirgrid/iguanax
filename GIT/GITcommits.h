#ifndef __GIT_COMMITS_H__
#define __GIT_COMMITS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITcommits
//
// Description:
//
// Returns all of the commits for a repository, for all of the branches
//
// Author: Tyler Brown 
// Date:   Tuesday 28 February 2023 - 10:26AM
// ---------------------------------------------------------------------------

class COLstring;
class COLvar;

void GITparseCommits(const COLstring& LogString, COLvar* pOut);

bool GITcommits(const COLstring& Dir, COLstring* pContent, COLstring* pError, bool Full, bool All);
bool GITlastCommitTime(const COLstring& Dir, COLstring* pContent, COLstring* pError);

#endif // end of defensive include
