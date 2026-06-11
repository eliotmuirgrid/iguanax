#ifndef __GITU_ADD_REMOTE_H__
#define __GITU_ADD_REMOTE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUaddRemote
//
// Description:
//
// This is used to add a remote repository to a repository.
//
// Author: Tyler Brown 
// Date:   Monday 06 March 2023 - 10:47AM
// ---------------------------------------------------------------------------

class COLstring;

bool GITUaddRemote(const COLstring& ComponentDir, const COLstring& RemoteUrl, COLstring* pError);

#endif // end of defensive include
