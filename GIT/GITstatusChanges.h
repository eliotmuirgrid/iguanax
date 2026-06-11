#ifndef GITSTATUSCHANGES_H
#define GITSTATUSCHANGES_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITstatusChanges.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   10/06/25 9:25 AM
//  ---------------------------------------------------------------------------
class COLstring;

bool GITstatusChanges(const COLstring& Dir, COLstring* pContent, COLstring* pError);

#endif  // end of defensive include
