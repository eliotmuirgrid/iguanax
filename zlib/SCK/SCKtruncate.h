#ifndef __SCK_TRUNCATE_H__
#define __SCK_TRUNCATE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKtruncate
//
// Description:
//
// Truncate buffer;
//
// Author: Eliot Muir 
// Date:   Monday 19 June 2023 - 04:48PM
// ---------------------------------------------------------------------------

class COLstring;

void SCKtruncateBuffer(int AmountToTruncate, COLstring* pBuffer);

#endif // end of defensive include
