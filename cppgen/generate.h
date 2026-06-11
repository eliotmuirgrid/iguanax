#ifndef __GENERATE_H__
#define __GENERATE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: generate
//
// Description:
//
// Routines to generate a header file.
//
// Author: Eliot Muir
// Date:   Sat 17 Sep 2022 15:13:46 EDT
// ---------------------------------------------------------------------------

class COLstring;

COLstring CPPprefix(const COLstring& Name);
COLstring CPPtimeStamp(const char* pPattern);
COLstring CPPconvertCamel(const COLstring& Camel);

bool CPPgenerateCode(const COLstring& Name, const COLstring& Author);

#endif // end of defensive include