#ifndef __CMPL_ADD_LIBRARY_H__
#define __CMPL_ADD_LIBRARY_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPLaddLibrary
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Tuesday 14 March 2023 - 04:47PM
// ---------------------------------------------------------------------------
struct CFGmap;
class COLwebRequest;
class SCKloop;

void CMPLmodifyModulesFile(const COLstring& ComponentId, const COLstring& LibraryName, const COLstring& GitUrl);
void CMPLaddLibrary(const COLwebRequest& Request, SCKloop* pLoop);

#endif // end of defensive include
