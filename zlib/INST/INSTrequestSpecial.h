#ifndef __INST_REQUEST_SPECIAL_H__
#define __INST_REQUEST_SPECIAL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTrequestSpecial
//
// Description:
//
// API to request upgrading to the latest beta build
//
// Author: Tyler Brown 
// Date:   Thursday 09 November 2023 - 07:26AM
// ---------------------------------------------------------------------------

class COLwebRequest;

void INSTupgradeRequestSpecial(const COLwebRequest& Request);

#endif // end of defensive include
