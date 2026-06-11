#ifndef HELPRESOLVE_H
#define HELPRESOLVE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HELPresolve.h
//
//  Description: Due to changes in our help system (IX-4108( it became difficult to determine whether we had a function
//  or method as well as finding the correct metatable in lua.
//
//  Author: Matthew Sobkowski
//  Date:   27/06/25 11:21 AM
//  ---------------------------------------------------------------------------
#include <COL/COLarray.h>

// Resolves the correct name to jump to by replacing the metatable prefix with our method notation
// i.e. used when clicking help title in intellisense dropdown
void HELPreplaceMetatablePrefix(COLstring* pName);

// parses a lua function into parts with the correct metatable name -- used to fill out intellisense dropdown help
void HELPresolveLua(const COLstring& String, COLarray<COLstring>* pFunctionParts, bool* pIsMethod);

#endif // end of defensive include
