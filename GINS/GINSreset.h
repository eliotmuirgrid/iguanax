#ifndef __GINS_RESET_H__
#define __GINS_RESET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSreset
//
// Description:
//
// Api to start a instance reset and api to check the status of an instance 
// reset
//
// Author: Tyler Brown 
// Date:   Tuesday 17 September 2024 - 08:13AM
// ---------------------------------------------------------------------------
class SCKloop;
class COLwebRequest;
class USRuser;

void GINSreset     (const COLwebRequest& Request, COLmap<COLstring, USRuser>* pUserMap, SCKloop* pLoop);
void GINSresetCheck(const COLwebRequest& Request);

#endif // end of defensive include
