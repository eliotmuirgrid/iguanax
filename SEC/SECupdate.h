#ifndef __SECUPDATE_H__
#define __SECUPDATE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SECupdate
//
// Description:
//
// Author: Matthew Sobkowski
// Date:   Thursday 28 November 2024 - 03:39PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class SCKloop;

void SECupdate(const COLwebRequest& Request, SCKloop* pLoop);

#endif // end of defensive include

