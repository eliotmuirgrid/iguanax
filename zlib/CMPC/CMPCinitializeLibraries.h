#ifndef CMPCINITIALIZELIBRARIES_H
#define CMPCINITIALIZELIBRARIES_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPCinitializeLibraries.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   28/05/25 2:56 PM
//  ---------------------------------------------------------------------------
class COLstring;
class GITUmodule;

bool CMPCinitializeLibrary(const COLstring& Username, const COLstring& ComponentId, const GITUmodule& Library,
                           const COLstring& ModuleName, COLstring* pErrorMessage);

bool CMPCinitializeAllLibraries(const COLstring& ComponentId, const COLstring& CommitId, const COLstring& Username, COLstring* pError);


#endif // end of defensive include
