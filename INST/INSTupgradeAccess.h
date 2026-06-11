#ifndef __INST_UPGRADE_ACCESS_H__
#define __INST_UPGRADE_ACCESS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTupgradeAccess
//
// Description:
//
// API to see if we have access to the upgrade server
//
// Author: Tyler Brown 
// Date:   Thursday 09 November 2023 - 07:26AM
// ---------------------------------------------------------------------------

class COLwebRequest;


void INSTupgradeAccess(const COLwebRequest& Request);

#endif // end of defensive include
