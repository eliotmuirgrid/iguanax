#ifndef __SDBA_RESUBMIT_H__
#define __SDBA_RESUBMIT_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBAresubmit.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   19/12/24 10:21 AM
//  ---------------------------------------------------------------------------
// #include <COL/COLclosure.h>
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>
#include <SDB/SDBfilter2.h>

class COLwebRequest;
class BLUAinstanceManager;
class SCKloop;

struct CFGmap;

class SDBAresubmitStatus {
  public:
   int       Count{0};
   COLstring Time;
   COLstring StartTime;
   COLstring StatusMessage;
   bool      HasError = false;
   void      reset();
};

class SDBAresubmitBulkState {
  public:
   SDBAresubmitBulkState() {}
   bool               m_Running{false};
   SDBfilterInfo2     m_Filter;
   COLstring          m_Target;
   SDBAresubmitStatus m_Status;
   int                m_Limit;
};

void SDBAresubmitStart(const COLwebRequest& Request, CFGmap* pMap, BLUAinstanceManager* pManager, SCKloop* pLoop);
void SDBAresubmitPoll(const COLwebRequest& Request);
void SDBAresubmitStop(const COLwebRequest& Request);

// Exposed for unit test
bool SDBAresubmitBulkValidate(const COLstring& User, CFGmap* pMap, const COLstring& Source,
                              const COLstring& Target, const SDBfilterInfo2& Filter, const COLaddress& Address);
#endif // end of defensive include
