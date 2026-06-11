#ifndef __SCK_WRITE_H__
#define __SCK_WRITE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKwrite
//
// Description:
//
// Write to connection.
//
// Author: Eliot Muir 
// Date:   Monday 19 June 2023 - 04:48PM
// ---------------------------------------------------------------------------

class SCKconnection;

int SCKwrite(SCKconnection* pConnection);

#endif // end of defensive include
