// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SCKtruncate
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 19 June 2023 - 04:48PM
// ---------------------------------------------------------------------------

#include <SCK/SCKtruncate.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SCKtruncateBuffer(int AmountWritten, COLstring* pBuffer){
   COL_FUNCTION(SCKtruncateBuffer);
   pBuffer->shift(AmountWritten);
}
