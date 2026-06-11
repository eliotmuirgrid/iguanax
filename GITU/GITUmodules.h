#ifndef __GITU_MODULES_H__
#define __GITU_MODULES_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUmodules
//
// Description:
//
// Routines to parse GIT submodule files - I do this to reduce dependencies on
// GIT
//
// Author: Eliot Muir 
// Date:   Monday 13 February 2023 - 03:11PM
// ---------------------------------------------------------------------------

#include <COL/COLmap.h>
#include <COL/COLarray.h>

class COLstring;

class GITUmodule{
public:
   GITUmodule() {}
   GITUmodule(const COLstring& Url, const COLstring& Branch) : GitUrl(Url), Branch(Branch) { }
   COLstring CommitId;
   COLstring GitUrl;
   COLstring Branch;
};

// TODO - rename this one - it's confusing with the next helper
COLstring GITUsubmoduleFile(const COLstring& GitUrl, const COLstring& Branch);
COLstring GITUsubmoduleFileInRepo(const COLstring& Dir);

// Loads the GitUrl and Name - but not the commit IDs
void GITUmoduleLoad(const COLstring& FileName, COLmap<COLstring, GITUmodule>* pList);

void GITUmoduleGetCommits(const COLstring& Dir, const COLstring& RelPath, const COLstring& CommitId, COLmap<COLstring, GITUmodule>* pList);

COLostream& operator<<(COLostream& Stream, const COLmap<COLstring, GITUmodule>& ModuleList);

#endif // end of defensive include
