#ifndef GINSREINITIALIZE_H
#define GINSREINITIALIZE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINSreinitialize.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   10/10/24 9:54 AM
//  ---------------------------------------------------------------------------
class SCKloop;
class COLwebRequest;

bool GINSremoveGitModulesFile(COLstring* pError);
void GINSreinitialize(const COLwebRequest& Request, SCKloop* pLoop);

#endif // end of defensive include
