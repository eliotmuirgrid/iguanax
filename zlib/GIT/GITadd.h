#ifndef __GIT_ADD_H__
#define __GIT_ADD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITadd
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Tuesday 04 April 2023 - 04:56PM
// ---------------------------------------------------------------------------

class COLstring;

bool GITadd(const COLstring& Dir, const COLstring& File, bool Stage, COLstring* pError);


#endif // end of defensive include
