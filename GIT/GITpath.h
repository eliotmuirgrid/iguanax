#ifndef __GIT_PATH_H__
#define __GIT_PATH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITpath
//
// Description:
//
// Grouping path functions here.
//
// Author: Eliot Muir 
// Date:   Tuesday 14 February 2023 - 08:41AM
// ---------------------------------------------------------------------------

class COLstring;

COLstring GITUcollectionFileName(const COLstring& GitUrl);

COLstring GITcacheDir();
COLstring GITbranchDir();
COLstring GITUblankTemplateDir();
COLstring GITgitTemplateDir(const COLstring& GitUrl, const COLstring& Branch="");
COLstring GITuserScratchDir(const COLstring& Username, const COLstring& GitUrl);

// ModuleName will be CHK etc.
COLstring GITUmoduleDirFull(const COLstring& ComponentId, const COLstring& ModuleName, bool Development);
COLstring GITUmoduleDir(const COLstring& ComponentId, const COLstring& ModuleName);
COLstring GITrunModuleDir(const COLstring& ComponentId, const COLstring& ModuleName);

#endif // end of defensive include