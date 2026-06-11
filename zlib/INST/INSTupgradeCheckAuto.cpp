//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: INSTupgradeCheckAuto.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   16/11/23 10:37 AM
//  ---------------------------------------------------------------------------
#include <ctime>

#include <INST/INSTupgradeCheckAuto.h>

#include <DIR/DIRpath.h>

#include <FIL/FILutils.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring INSTformatTime(int minutes, int seconds) {
   tm timeStruct;
   timeStruct.tm_min = minutes;
   timeStruct.tm_sec = seconds;
   char buffer[6]; // "mm:ss\0"
   strftime(buffer, sizeof(buffer), "%M:%S", &timeStruct);
   return COLstring(buffer);
}

static COLstring INSTtimeRemaining(time_t EpochTime, time_t CurrentTime) {
   COL_FUNCTION(INSTisOneHour);
   time_t timeDifference = difftime(EpochTime, CurrentTime);
   if (timeDifference > 0 && timeDifference <= 3600) {
      int minutes = timeDifference / 60;
      int seconds = timeDifference % 60;
      return INSTformatTime(minutes, seconds);
   } else {
      return "";
   }
}


static bool INSTupgradeCheckImpl(COLvar* pResult, COLstring* pErr) {
   COL_FUNCTION(INSTupgradeCheckImpl);
   if(!FILfileExists(DIRfileConfigAutoUpgrade())) { return false; }
   try {
      COLstring Data;
      FILreadFile(DIRfileConfigAutoUpgrade(), &Data);
      if (!pResult->parse(Data)) {
         *pErr = "File has invalid format -- expected json";
         return false;
      }
      time_t SavedTime   = atoll((*pResult)["time"].asString().c_str()) / 1000;
      time_t CurrentTime = time(0);
      (*pResult)["warning"] = INSTtimeRemaining(SavedTime, CurrentTime);
      COL_VAR4(*pResult, SavedTime, CurrentTime, *pResult);
      return true;
   }catch (const COLerror& E) {
      *pErr = E.description();
      return false;
   }
}

// upgrade/check_auto
void INSTupgradeCheckAuto(const COLwebRequest& Request) {
   COL_FUNCTION(INSTupgradeCheckAuto);
   COLvar Result;
   COLstring Err;
   bool Success = INSTupgradeCheckImpl(&Result, &Err);
   Success ? COLrespondSuccess(Request.Address, Result) : COLrespondError(Request.Address, Err);
}