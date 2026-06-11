//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITSapi.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   4/27/23 1:38 PM
//  ---------------------------------------------------------------------------
#include <GITS/GITSapi.h>

#include <GITS/GITSdownload.h>
#include <GITS/GITSexists.h>
#include <GITS/GITSinstall.h>
#include <GITS/GITSdownloadPresent.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void GITSapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap) {
   COL_FUNCTION(GITSapi);
   ApiMap.add("/git/exists",   &GITSexists);
   ApiMap.add("/git/download", &GITSdownload);
   ApiMap.add("/git/download/present", &GITSdownloadPresent);
   ApiMap.add("/git/install",  &GITSinstall);
}