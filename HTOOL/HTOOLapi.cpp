// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HTOOLapi.cpp
//
//  Description
//
//  Author: Matthew Sobkowski
//  Date:   10/19/22 1:10 PM
// ---------------------------------------------------------------------------
#include <HTOOL/HTOOLapi.h>
#include <HTOOL//HTOOLfileDiff.h>
#include <WEB/WEBserver.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void HTOOLapiAsync(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(HTOOLapiAsync);
   ApiMap.add("/htool/file_diff", HTOOLfileDiff);
}
