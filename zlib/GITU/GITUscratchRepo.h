#ifndef __GITU_SCRATCH_REPO_H__
#define __GITU_SCRATCH_REPO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUscratchRepo
//
// Description:
//
// For some GIT command operations we need to do them within a valid git repo.
//
// GITUscratchRepo tries to create a empty repo for this purpose.
//
// Author: Eliot Muir 
// Date:   Monday 27 March 2023 - 10:06AM
// ---------------------------------------------------------------------------

#include <COL/COLstring.h>

void GITUscratchRepo(const COLstring& Dir, const COLstring& Username="");
void GITUscratchRepoRemove(const COLstring& Dir);

#endif // end of defensive include
