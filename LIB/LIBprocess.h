#ifndef LIBPROCESS_H
#define LIBPROCESS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LIBprocess.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   10/06/25 1:07 PM
//  ---------------------------------------------------------------------------
#include <COL/COLmap.h>
class GITUmodule;

struct GSVlink;
struct LIBusage;

void LIBsortUsage(COLarray<LIBusage>* pUsages);
void LIBprocessCatalogue(const COLmap<COLstring, GSVlink>& KeyMap, COLvar* pOut);
void LIBprocessComponentModules(const COLstring& User, const COLstring& Guid, const COLmap<COLstring, GSVlink>& KeyMap,
                                COLvar* pOut);
COLstring LIBconstructUrlKey(const COLstring& GitUrl, GSVlink* pLink);

#endif // end of defensive include
