#ifndef __DEP_CONTAINS_H__
#define __DEP_CONTAINS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DEPcontains
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Wednesday 16 April 2025 - 08:55AM
// ---------------------------------------------------------------------------
class COLstring;

COLstring DEPcontainsMessage(const COLstring& FunctionName);

bool DEPdirContainsFunc(const COLstring Dir, const COLstring FunctionName);

#endif // end of defensive include
