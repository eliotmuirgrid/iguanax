#ifndef GINSLASTPUSH_H
#define GINSLASTPUSH_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINSlastPush.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   17/09/24 3:29 PM
//  ---------------------------------------------------------------------------
class COLwebRequest;

struct GINScommitInfo;

void GINSlastPush(const COLwebRequest& Request, GINScommitInfo* pCommitInfo);

#endif // end of defensive include
