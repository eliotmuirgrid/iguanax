#ifndef __SCK_ON_CONNECT_H__
#define __SCK_ON_CONNECT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKonConnect
//
// Description:
//
// Teasing out this callback handler into it's own CPP.
//
// Author: Eliot Muir 
// Date:   Monday 19 June 2023 - 03:30PM
// ---------------------------------------------------------------------------

class SCKconnection;

bool SCKonConnect(SCKconnection* pConnection);


#endif // end of defensive include
