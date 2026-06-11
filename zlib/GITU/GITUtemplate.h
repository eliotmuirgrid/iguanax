#ifndef __GITU_TEMPLATE_H__
#define __GITU_TEMPLATE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUtemplate
//
// Description:
//
// Routine to assemble a template based from locally cached resources
//
// Author: Eliot Muir 
// Date:   Tuesday 14 February 2023 - 12:34PM
// ---------------------------------------------------------------------------

class COLstring;
// class COLmap;  
#include <COL/COLmap.h>
class GITUmodule;

// Creates template in given directory
// Assumes that the relevant git repos have been locally cached
bool GITUtemplateCreate(const COLstring& Dir, const COLstring& GitUrl, const COLstring& Branch, COLstring* pError);
void GITUcreateDirCopy(const COLstring& SourceDir, const COLstring& DestDir);
bool GITUinitSubmoduleAndUpstream(const COLstring& Dir, const COLmap<COLstring, GITUmodule>& ModuleList, COLstring* pError);
void GITUcheckoutSubmoduleCommits(const COLstring& Dir, const COLmap<COLstring, GITUmodule>& ModuleList, COLstring* pError);
void GITUintializeSubmodulesSubmodules(const COLstring& Dir, const COLstring& RelPath, COLstring* pError);

#endif // end of defensive include
