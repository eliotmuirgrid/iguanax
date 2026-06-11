#ifndef GINSUTILS_H
#define GINSUTILS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINSutils.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   17/09/24 1:22 PM
//  ---------------------------------------------------------------------------
class COLstring;
class COLvar;

void GINSchangeKeyPermissions();

void GINSsetError(const COLstring& Error, COLvar* pOut);

bool GINSloadAndParse(COLvar* pConfig, COLstring* pErr);

bool GINSstatusClean(const COLstring& Dir, COLstring* pOut, COLstring* pErr);
bool GINShashExists(const COLstring& Dir, const COLstring& Hash, COLstring* pOut, COLstring* pErr);

void GINSinitializeSubmodules(const COLstring& Dir, COLstring* pError);
void GINSinitializeSubmodules(const COLstring& Dir, const COLstring& Username, COLstring* pError);

void GINSpullComponentRepos(const COLstring& Dir, const COLstring& Username, COLstring* pError);

#endif // end of defensive include
