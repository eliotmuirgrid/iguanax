#ifndef __GIT_COMMIT_H__
#define __GIT_COMMIT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITcommit
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Wednesday 05 April 2023 - 06:37PM
// ---------------------------------------------------------------------------

class COLstring;

bool GITcommit(const COLstring& Username, const COLstring& Dir, const COLstring& Flags, const COLstring& Message, COLstring* pError);


#endif // end of defensive include
