#ifndef __NOTI_LOAD_H__
#define __NOTI_LOAD_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: NOTIload.cpp
//
//  Description: Notification config api handler
//
//  Author: Matthew Sobkowski
//  Date:   8/31/22, 10:53 AM
// ---------------------------------------------------------------------------
class COLwebRequest;
class NOTImodel;

void NOTIload(const COLwebRequest& Request, NOTImodel* pNoti);

#endif // end of defensive include