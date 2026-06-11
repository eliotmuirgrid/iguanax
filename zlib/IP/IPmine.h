#ifndef __IP_MINE_H__
#define __IP_MINE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IPmine
//
// Description:
//
// Helper to fetch IP address - makes a web request to an external service.
//
// Author: Eliot Muir 
// Date:   Tuesday 26 September 2023 - 08:39AM
// ---------------------------------------------------------------------------

class COLstring;

bool IPmine(COLstring* pAddress);

#endif // end of defensive include
