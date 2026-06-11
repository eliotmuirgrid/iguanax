#ifndef __GINS_USER_H__
#define __GINS_USER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSuser
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Monday 16 September 2024 - 07:57AM
// ---------------------------------------------------------------------------
class SCKloop;
class COLwebRequest;
class USRuser;

void GINSuser(const COLwebRequest& Request, SCKloop* pLoop, COLmap<COLstring, USRuser>* pUserMap);


#endif // end of defensive include
