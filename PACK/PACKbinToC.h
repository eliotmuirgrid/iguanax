#ifndef __PACK_BIN_TO_C_H__
#define __PACK_BIN_TO_C_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PACKbinToC
//
// Description:
//
// Convert binary data to C code format like xxd
//
// Author: Eliot Muir 
// Date:   Wednesday 16 August 2023 - 10:07AM
// ---------------------------------------------------------------------------

class COLstring;

void PACKbinToC(const COLstring& In, COLstring* pOut);

#endif // end of defensive include
