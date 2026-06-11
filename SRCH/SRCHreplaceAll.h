#ifndef SRCHREPLACEALL_H
#define SRCHREPLACEALL_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SRCHreplaceAll.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   30/01/25 4:43 PM
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>
class COLwebRequest;

void SRCHreplaceAllImpl(const COLvar& Matches, const COLstring& Query, const COLstring& Replacement,
                        COLmap<COLstring, COLstring>* pFileContents);
void SRCHreplaceAllInit(const COLstring& Path, const COLstring& Query, const COLstring& Replacement, bool MatchCase,
                        bool MatchFull, const COLvar& Selections = COLvar());
void SRCHreplaceAll(const COLwebRequest& Request);

#endif  // end of defensive include