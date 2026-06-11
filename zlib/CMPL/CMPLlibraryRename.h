#ifndef CMPLLIBRARYRENAME_H
#define CMPLLIBRARYRENAME_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPLlibraryRename.h
//
//  Description: Rename libraries
//
//  Author: Matthew Sobkowski
//  Date:   20/12/23 11:21 AM
//  ---------------------------------------------------------------------------

struct CFGmap;
class COLwebRequest;
class SCKloop;

void CMPLlibraryRename(const COLwebRequest& Request, SCKloop* pLoop);

bool CMPLrenameLibraryFiles(const COLstring& RootDir, const COLstring& Dir, const COLstring& OldLibraryName, const COLstring& NewLibraryName, COLstring* pError);

#endif // end of defensive include
