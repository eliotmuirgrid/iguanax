#ifndef __PACKWEBPACK_H__
#define __PACKWEBPACK_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PACKwebPack.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   6/5/23 1:56 PM
//  ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLauto.h>
#include <COL/COLarray.h>

class COLstring;
class COLvar;
class SCKloop;

void PACKwebPack(const COLarray<COLstring>& DirList, COLclosure1<const COLvar&>* pOnComplete);

#endif // end of defensive include
