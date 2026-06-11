#ifndef __GITU_REPOSITORY_H__
#define __GITU_REPOSITORY_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUrepository
//
// Description:
//
// Gives you the origin of the git repository
//
// Author: Tyler Brown 
// Date:   Thursday 23 February 2023 - 03:31PM
// ---------------------------------------------------------------------------
#include <GITU/GITUlist.h>

class COLstring;

// New cleaner API to use.
COLstring GITUrepositoryRemoteUrl(const COLstring& Dir);

bool GITUrepository(const COLstring& Dir, const COLstring& Tag, COLstring* pContent);

bool GITUgetUpstreamList(const COLstring& Folder, GITUlist* pList, COLstring* pError);

#endif // end of defensive include
