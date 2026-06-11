#ifndef SRCHSEARCH_H
#define SRCHSEARCH_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SRCHsearch.h
//
//  Description: Search similar to vscode
//  If matches are found, the result COLvar will have a "matches" key that contains a list of all matches with important
//  information such as line number, position in file, filename, and line content. This is mainly used in the context of
//  the translator which is why we start lines with 1. Do not use on main thread as file I/O occurs.
//  NOTE: See IX-3809 for explanation for vscode search implementation
//
//  Author: Matthew Sobkowski Date:   30/01/25 12:27 PM
//  ---------------------------------------------------------------------------
class COLstring;
class COLvar;
class COLwebRequest;

void SRCHsearch(const COLstring& Path, const COLstring& Query, bool MatchCase, bool MatchFullWord, COLvar* pResults);

#endif  // end of defensive include
