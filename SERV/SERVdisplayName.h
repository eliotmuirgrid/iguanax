#ifndef __SERV_DISPLAY_NAME_H__
#define __SERV_DISPLAY_NAME_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVdisplayName
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Wednesday 23 August 2023 - 02:07PM
// ---------------------------------------------------------------------------

class COLstring;

void SERVsetDisplayName(const COLstring& DisplayName);

COLstring SERVdisplayName();

#endif // end of defensive include
