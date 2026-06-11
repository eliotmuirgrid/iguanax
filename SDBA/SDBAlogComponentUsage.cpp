//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBAlogComponentUsage.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   17/07/25 11:52 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <SDB/SDBcapi.h>
#include <SDBA/SDBAlogComponentUsage.h>
COL_LOG_MODULE;

static void SDBAgetComponentUsage(SDBlogStats Stats, SDBlogStats Stats2, const COLaddress Address) {
   COL_FUNCTION(SDBAgetComponentUsage);
   bool AltLogDirSet = !SDBroot2().is_null();
   COLvar Result;
   Result.setMapType();
   for(const auto& it : Stats.ComponentLogSizes) {
      COLvar Data;
      Data["component"] = it.first;
      Data["name"]      = it.second.Name;
      Data["log_size"]  = COLuint64ToString(it.second.Size);
      Data["path"]      = SDBdir(it.first, SDBroot());
      if (AltLogDirSet) {
         Data["alt_log_size"]  = COLuint64ToString(Stats2.ComponentLogSizes[it.first].Size);
         Data["alt_path"]      = SDBdir(it.first, SDBroot2());
      }
      Result[it.first]  = Data;
   }
   COLrespondSuccess(Address, Result);
}

// /log/component/usage
void SDBAlogComponentUsage(const COLwebRequest& Request) {
   COL_FUNCTION(SDBAlogComponentUsage);
   SDBIgetLogStats(COLnewClosure2(&SDBAgetComponentUsage, Request.Address));
}