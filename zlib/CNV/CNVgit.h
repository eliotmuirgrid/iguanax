#ifndef __CNV_GIT_H__
#define __CNV_GIT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 2024 Eliot Muir.  All Rights Reserved
//
// Module: CNVgit
//
// Description:
//
// This is code to copy an Iguana 6 repo and convert it.
//
// Author: Eliot Muir 
// Date:   Sunday 15 December 2024 - 04:58PM
// ---------------------------------------------------------------------------

class COLstring;
class COLostream;

void CNVclean(COLostream& Out);
// Give it the working dir
void CNVclone(const COLstring& Dir, COLostream& Out);

#endif // end of defensive include
