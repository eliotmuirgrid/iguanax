#ifndef __NOTI_SAVE_H__
#define __NOTI_SAVE_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: NOTIsave.cpp
//
//  Description: Save notification config
//
//  Author: Matthew Sobkowski
//  Date:   8/31/22, 10:53 AM
// ---------------------------------------------------------------------------
class COLwebRequest;
class NOTImodel;
class SCKloop;
class CFGmap;

void NOTIsave(const COLwebRequest& Request, NOTImodel* pNoti, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include