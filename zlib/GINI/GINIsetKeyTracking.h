#ifndef GINISET_H
#define GINISET_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINIset.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   21/05/25 12:10 PM
//  ---------------------------------------------------------------------------
class COLwebRequest;

bool GINIsetKeyTrackingImpl(const COLstring& User, bool AddSection, COLarray<COLstring>* pIgnoreLines,
                            COLstring* pErr);
void GINIsetKeyTracking(const COLwebRequest& Request);

#endif // end of defensive include
