//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAautoStart
//
// Author: John Q
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include <BLUA/BLUAautoStart.h>
#include <BLUA/BLUAsafe.h>
#include <BLUA/BLUAstart.h>
#include <CFG/CFGmap.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <DRX/DRXutils.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

bool BLUAautoStartEnabled(const COLarray<COLstring>& Tags) {
   COL_FUNCTION(BLUAautoStartEnabled);
   COL_VAR(Tags.size());
   static COLstring AutoStartTag = "#autostart";
   for(const auto& it : Tags) { if(it == AutoStartTag) { return true; } }
   return false;
}

static void BLUAlogFailedToStart(const COLarray<COLstring>& FailedToStart, const COLstring& Reason){
   COL_FUNCTION(BLUAlogFailedToStart);
   if (FailedToStart.size() == 1) {
      SDBIlogService("Component " + FailedToStart[0] + " failed to automatically start due to a license issue. " + Reason, "#error #startup");
      return;
   }
   COLstring Names;
   COLjoin(&Names, FailedToStart, ", ");
   SDBIlogService("Components " + Names + " failed to automatically start due to a license issue. " + Reason, "#error #startup");
}

struct BLUAautoStartComplete {
   COLclosure0* pDone;
   int          StartedCount;
};

static void BLUAautoStartFinished(BLUAautoStartComplete* pComplete) {
   COL_FUNCTION(BLUAautoStartFinished);
   if(pComplete->StartedCount > 0) {
      if(pComplete->StartedCount == 1) {
         SDBIlogService("Auto started 1 component.", "#startup");
         COLcout << "Auto started 1 component." << newline;
      } else {
         SDBIlogService("Auto started " + COLintToString(pComplete->StartedCount) + " components.", "#startup");
         COLcout << "Auto started " << pComplete->StartedCount << " components." << newline;
      }
   }

   pComplete->pDone->runAndDelete();
   delete pComplete;
}

void BLUAstartComponents(COLclosure0* pDone, SCKloop* pLoop, CFGmap* pMap, BLUAinstanceManager* pManager) {
   COL_FUNCTION(BLUAstartComponents);
   if(BLUAsafeMode()) {
      SDBIlogService("Component auto-start is disabled in safe mode.", "#startup");
      COLcout << "Component auto-start is disabled in safe mode." << newline;
      pDone->runAndDelete();
      return;
   }

   COLarray<COLstring> ComponentGuids, FailedToStart;
   COLstring           Reason;
   int                 Count = 0;

   for(const auto& it : pMap->ConfigMap) {
      if(BLUAautoStartEnabled(it.second.CoreConfig.Tags)) {
         if(!DRXcomponentCountCheck(Count + 1, &Reason)) {
            SDBIlogComponent("Iguana failed to automatically start the component due to a license issue. " + Reason,
                             it.first, "#error #startup");
            FailedToStart.push_back(it.second.CoreConfig.Name);
         } else {
            Count++;
            SDBIlogComponent("Iguana is automatically starting the component.", it.first, "#startup");
            ComponentGuids.push_back(it.first);
         }
      }
   }
   if(FailedToStart.size() != 0) { BLUAlogFailedToStart(FailedToStart, Reason); }
   if(ComponentGuids.size() == 0) {
      COL_TRC("No components to auto-start.");
      pDone->runAndDelete();
      return;
   }

   auto pComplete          = new BLUAautoStartComplete();
   pComplete->pDone        = pDone;
   pComplete->StartedCount = ComponentGuids.size();
   COLclosure0* pFinished  = COLnewClosure0(&BLUAautoStartFinished, pComplete);
   BLUAstartComponentList(ComponentGuids, false, pManager, pMap, pLoop, pFinished, 20, 1000);
}
