#ifndef GINSROLLBACK_H
#define GINSROLLBACK_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINSrollback.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   19/09/24 11:46 AM
//  ---------------------------------------------------------------------------
class SCKloop;
class COLwebRequest;

void GINSrollback(const COLwebRequest& Request, SCKloop* pLoop);

#endif // end of defensive include
