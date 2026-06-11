#ifndef __PACKATTACH_H__
#define __PACKATTACH_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PACKattach.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   6/16/23 10:52 AM
//  ---------------------------------------------------------------------------
#include <COL/COLstring.h>

class COLvar;

const COLstring PACK_MAGIC_STRING("ifw🦎wfi");

bool PACKextractFromExe(const COLstring& Binary, COLvar* pData, COLstring* pError);
void PACKattachToExe(const COLvar& Result, COLstring IguanaBinary);

// unit test
bool PACKfind(const COLstring& Buffer, const COLstring& SubStr, int* pPosition);

#endif // end of defensive include
