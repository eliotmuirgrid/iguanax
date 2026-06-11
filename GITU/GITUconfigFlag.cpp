// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUconfigFlag
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 15 November 2023 - 01:17PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <GITU/GITUconfigFlag.h>
#include <GITU/GITUutils.h>
COL_LOG_MODULE;

COLstring GITUconfigFlag(const COLstring& Username){
   COL_FUNCTION(GITUconfigFlag);
   if(Username.is_null()){
      COL_TRC("No username so no config flag to provide");
      return "";
   }
   COLstring FileName = DIRconfigUserFile(Username);
   COLstring ConfigFlag = " -c include.path=" + GITUquoteArgument(FileName);
   COL_VAR(ConfigFlag);
   return ConfigFlag;
}