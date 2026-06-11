#ifndef __INI_COMPARE_H__
#define __INI_COMPARE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INIcompare
//
// Description:
//
// Compare two config files
//
// Author: Eliot Muir 
// Date:   Sunday 26 March 2023 - 11:47AM
// ---------------------------------------------------------------------------

class COLstring;
// This will load in an initialization file and save it, this will have the impact of alphabetically
// sorting the sections and keys which makes comparison easier.
void INIsortFile(const COLstring& File);

#endif // end of defensive include
