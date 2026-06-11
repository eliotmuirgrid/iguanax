#ifndef PACKHELP_H
#define PACKHELP_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PACKhelp.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/06/25 10:27 AM
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>

COLstring PACKreplaceHelpPrefix(const COLstring& RootDir, const COLstring& File);
void PACKaddHelpHash(COLmap<COLstring, COLstring>* pFileMap);

#endif // end of defensive include
