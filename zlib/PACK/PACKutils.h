#ifndef PACKUTILS_H
#define PACKUTILS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PACKutils.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/06/25 10:17 AM
//  ---------------------------------------------------------------------------
#include <COL/COLarray.h>
#include <COL/COLmap.h>

COLstring PACKfindRoot(const COLarray<COLstring>& DirList);
void PACKconcatFiles(const COLstring& RootDir, COLmap<COLstring, COLstring>* pFileMap);
void PACKgenerateIndex(const COLstring& IndexFile, COLmap<COLstring, COLstring>* pFileMap);
bool PACKisGzippedFile(const COLstring& File);

COLvar PACKmapToVar(const COLmap<COLstring, COLstring>& FileMap);

#endif // end of defensive include
