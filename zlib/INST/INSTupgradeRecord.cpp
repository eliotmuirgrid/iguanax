// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTupgradeRecord
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Wednesday 08 January 2025 - 12:28PM
// ---------------------------------------------------------------------------

#include <INST/INSTupgradeRecord.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <SDB/SDBcapi.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#define INST_RESTART_TIME_BUFFER 120 // 2 minutes in seconds

bool INSTcreateUpgradeRecord(COLstring* pError){
   COL_FUNCTION(INSTcreateUpgradeRecord);
   try {
      COLstring Location = FILpathAppend(DIRotherDir(), "upgrade_record.txt");
      time_t Now = time(NULL);
      COLstring Data = COLintToString(Now);
      COL_TRC("Writing upgrade record, time: " << Data);
      FILwriteFile(Location, Data);
   } catch (const COLerror& Error) {
      *pError = Error.description();
      return false;
   }
   return true;
}

static bool INSTcompareUpgradeRecordTime(const COLstring& Location){
   COL_FUNCTION(INSTcompareUpgradeRecordTime);
   COLstring Content;
   FILreadFile(Location, &Content);
   COL_VAR(Content); // expect this to be an epoch timestamp
   time_t Now = time(NULL);
   long long NowAsLongLong = (long long)Now;
   long long UpgradeTime = atoll(Content.c_str());
   long long Diff = NowAsLongLong - UpgradeTime;
   COL_TRC("Upgrade record time: " << UpgradeTime << ", current time: " << NowAsLongLong);
   SDBIlogService("Iguana service started up in " + COLintToString(Diff) + " seconds after upgrade", "#upgrade");
   if (Diff > INST_RESTART_TIME_BUFFER) {
      COL_TRC("Server did not come up within 2 minutes after upgrade");
      return false;
   }
   COL_TRC("Server restarted after upgrade");
   return true;
}

void INSThandleUpgradeRecord(){
   COL_FUNCTION(INSThandleUpgradeRecord);
   COLstring Location = FILpathAppend(DIRotherDir(), "upgrade_record.txt");
   if (!FILfileExists(Location)) {
      COL_TRC("No upgrade_record.txt found.");
      return;
   }
   try{
      bool ServerRestoreOk = INSTcompareUpgradeRecordTime(Location);
      if (!ServerRestoreOk){
         SDBIlogService("It appears that Iguana did not automatically restart after upgrade.", "#error #upgrade");
      }
      COL_TRC("Safe to delete upgrade record file");
      FILremove(Location);
   } catch(const COLerror& Error) {
      Error.description();
   }
}