//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: ENVstartup.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/29/23 11:31 AM
//  ---------------------------------------------------------------------------
#include <ENV/ENVstartup.h>
#include <ENV/ENVparse.h>
#include <ENV/ENVfile.h>
#include <ENVC/ENVCcache.h>

#include <PRO/PROenv.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void ENVstartup() {
   COL_FUNCTION(ENVstartup);
   ENVCcacheInitOS();
   ENVmap EnvMap;
   ENVfileLoad(&EnvMap);
   for(const auto& it: EnvMap.Map) {
      COL_TRC("Setting environment variable " << it->first << "=" << it->second);
      PROsetenv(it->first, it->second);
      ENVCcacheSet(it.first, it.second);
   }
}