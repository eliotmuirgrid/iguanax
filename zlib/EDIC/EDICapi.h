#ifndef __EDIC_REGISTER_API_H__
#define __EDIC_REGISTER_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Register API for EDI backend.
//
// Author:   Eliot Muir
// Date:     26th July 2022
//---------------------------------------------------------------------------

#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

void EDICapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include.
