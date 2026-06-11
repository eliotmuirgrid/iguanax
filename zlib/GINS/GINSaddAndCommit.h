#ifndef GINSADDANDCOMMIT_H
#define GINSADDANDCOMMIT_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINSaddAndCommit.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   07/10/24 1:36 PM
//  ---------------------------------------------------------------------------
class SCKloop;
class COLwebRequest;
class COLvar;

void GINSaddSettings(COLstring* pErr);
void GINSaddReports(COLstring* pErr);
void GINSaddUsers(COLstring* pErr);
void GINSaddOther(COLstring* pErr);
bool GINSaddAndCommitImpl(COLvar In, COLstring* pErr);
void GINSaddAndCommit(const COLwebRequest& Request, SCKloop* pLoop);

#endif // end of defensive include
