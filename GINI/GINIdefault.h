#ifndef GINIDEFAULT_H
#define GINIDEFAULT_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINIdefault.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   22/05/25 10:58 AM
//  ---------------------------------------------------------------------------
class COLstring;

bool GINIdefault(COLstring* pErr, bool ForceKeyTracking = false);

void GINIcheckDefaultExists(COLarray<COLstring>* pIgnoreLines);

#endif // end of defensive include
