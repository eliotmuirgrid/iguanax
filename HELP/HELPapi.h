#ifndef __HELP_API_H__
#define __HELP_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: HELPapi
//
// Description:
//
// APIs for the Help system
//
// Author: Orkhan Amikishiyev
//---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

void HELPapiWorker(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);

#endif // end of defensive include