#ifndef __ENV_API_H__
#define __ENV_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: ENVapi
//
// Description:
//
// API for the environmental variables page
//
// Author: Eliot Muir 
// Date:   Thursday 23 March 2023 - 11:50AM
// ---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

void ENVapiAsync(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include
