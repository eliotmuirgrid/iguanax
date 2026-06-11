#ifndef __NOTI_NOTI_MODEL_H__
#define __NOTI_NOTI_MODEL_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: NOTImodel
//
//  Description:
//
//  POD datastructure for notification configuration
//
//  Author: Eliot Muir
//  Date:  August 31st 2022
// ---------------------------------------------------------------------------
#include <ctime>

#include <COL/COLstring.h>
#include <COL/COLarray.h>
#include <COL/COLmap.h>
#include <SCK/SCKtimer.h>
#include "../COL/COLvar.h"
#include "../COL/COLstring.h"

class BLUAinstance2;
class SCKloop;

enum class NOTIpriority {
   LOW = 0,
   MEDIUM = 1,
   HIGH = 2,
};

enum class NOTIruleType {
   INACTIVE = 0,
   QUEUE = 1,
   HAS_ERRORS = 2,
   LOG_CONTAINS = 3
};

struct NOTIerror{
   COLstring ComponentId;
   COLstring Reason;
};

class NOTIrule {
 public:
   NOTIrule() : Type(NOTIruleType::INACTIVE) {}
   NOTIrule(const COLarray<COLstring>& _Tags, NOTIruleType _Type, COLstring _Threshold)
      : Tags(_Tags), Type(_Type), Threshold(_Threshold) {}
   COLarray<COLstring> Tags;
   NOTIruleType Type = NOTIruleType::HAS_ERRORS;
   COLstring Threshold; // Threshold will be minutes for INACTIVE, messages for QUEUE, and pattern for LOG_CONTAINS
   NOTIpriority Priority = NOTIpriority::LOW;
   int Limit             = 6; // messages per hour
   COLmap<COLstring, COLarray<time_t>> MessageTimes;
};

class NOTImodel{
public:
   COLstring ComponentId;   // Component we send notifications to
   COLarray<NOTIrule> Rules;
   // IX-783 Add SCKtimer object here
   SCKtimer Timer;
   SCKloop* pLoop;
   COLarray<NOTIerror> ErrorList;  // IX-783 Why is this persistent?
   NOTImodel(SCKloop* Loop) : Timer(Loop), pLoop(Loop) {};
   bool IsOn = false;
};

COLostream& operator<<(COLostream& Stream, const NOTImodel& Model);
COLostream& operator<<(COLostream& Stream, const NOTIrule&  Rule);

COLstring NOTIpriorityToString(const NOTIpriority& Priority);
COLstring NOTIruleToString(const NOTIruleType& Rule);

// These are for loading from disc - these APISs are built for backward compatibility with earlier product versions.
void NOTImodelFromDiscVar(NOTImodel* pOut, const COLvar& Input);
void NOTImodelToDiscVar(COLvar* pOut, const NOTImodel& Input);

// DELIBERATELY making the interface to the web separate from how we serialize to disc.
// For web api
bool NOTImodelFromWebVar(NOTImodel* pOut,const COLstring& Component, bool IsOn, const COLvar& Rules, COLstring* pErr);
void NOTImodelToWebVar(COLvar* pOut, const NOTImodel& Input);

void NOTIcreateDefault(NOTImodel* pModel);

#endif // end of defensive include