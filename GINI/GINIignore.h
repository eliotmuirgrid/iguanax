#ifndef GINIIGNORE_H
#define GINIIGNORE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINIignore.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   22/05/25 10:37 AM
//  ---------------------------------------------------------------------------
#include <COL/COLarray.h>

extern const char* GINI_KEY_SECTION;
extern const char* GINI_DEFAULT_SECTION;

bool GINIsaveIgnore(const COLstring& Path, const COLarray<COLstring>& Lines, COLstring* pErr);
bool GINIfindSection(const COLarray<COLstring>& IgnoreLines, const COLstring& Section);
bool GINIaddAndCommitIgnore(const COLstring& User, COLstring* pErr);

void GINIremovePatterns(COLarray<COLstring>* pIgnoreLines, const COLarray<COLstring>& PatternsToRemove);

COLarray<COLstring> GINIreadIgnoreLines(const COLstring& GitIgnoreFile);

void GINIprocessIgnore(COLarray<COLstring>* pGitIgnoreLines, const COLstring& Section, const bool AddSection,
                       const COLarray<COLstring>& Patterns);

#endif // end of defensive include
