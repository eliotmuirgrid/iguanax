#ifndef __SCK_ON_CLOSE_H__
#define __SCK_ON_CLOSE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKonClose
//
// Description:
//
// On close handler.
//
// Author: Eliot Muir 
// Date:   Monday 19 June 2023 - 04:35PM
// ---------------------------------------------------------------------------

class SCKconnection;

bool SCKonClose(SCKconnection* pConnection);

#endif // end of defensive include
