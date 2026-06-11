#ifndef __SECaccess_H__
#define __SECaccess_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SECaccess
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski
// Date: 2024-11-28
// ---------------------------------------------------------------------------
class COLstring;
class COLvar;

void SECfreePointers();
void SECinit(bool FreshInstall=true);
bool SECset(const COLvar& Var);
bool SECload(COLvar* pOut, COLstring* pErr);
bool SECsave(const COLvar& Var, COLstring* pErr);
bool SECvalidate(const COLstring& Path, COLstring* pErr, const COLstring& Command="");
bool SECosExecuteRestricted();
COLvar SECtoVar();

#endif  // end of defensive include
