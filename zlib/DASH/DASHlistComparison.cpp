//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DASHlistComparison.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   08/07/25 1:49 PM
//  ---------------------------------------------------------------------------
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLstring.h>
#include <COL/COLvarUtils.h>
#include <ICOL/ICOLcomponent.h>
#include <SDB/SDBcapi.h>
COL_LOG_MODULE;

#include <DASH/DASHqueuePosition.h>
#include <DASH/DASHlistComparison.h>

#include <cctype>

int DASHtoUpperMod(int Char){
   COL_FUNCTION(DASHtoUpperMod);
   int Result = toupper(Char);
   if (Result == '_'){
      Result = 'A' -1;
   }
   return Result;
}

// See IX-831 for the rationale for this.  This comparison function will put names with _ at the top - so _AAA will appear above AAA
int DASHicompare(const char* s1, const char* s2){
   COL_FUNCTION(DASHicompare);
   const unsigned char* us1 = (const unsigned char*)s1;
   const unsigned char* us2 = (const unsigned char*)s2;
   int i1, i2; // must be int
   for (; (i1 = DASHtoUpperMod(*us1)) == (i2 = DASHtoUpperMod(*us2)); ++us1, ++us2) {
      if (i1 == 0) {
         return 0;
      }
   }
   return i1 - i2;
}

bool DASHlastCompare(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column) {
   if(pComponent1->Status.LastActivity == pComponent2->Status.LastActivity) { return DASHnameCompare(pComponent1, pComponent2, Ascending, Column); }
   return (pComponent1->Status.LastActivity < pComponent2->Status.LastActivity) == Ascending;
}

bool DASHmpsiCompare(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column) {
   COL_FUNCTION(DASHmpsiCompare);
   float Mps1 = SDBIgetComponentStats(pComponent1->CoreConfig.Guid);
   float Mps2 = SDBIgetComponentStats(pComponent2->CoreConfig.Guid);
   if (Mps1 == Mps2) { return DASHnameCompare(pComponent1, pComponent2, Ascending, Column); }
   bool Result = Mps1 < Mps2;
   COL_TRC(pComponent1->CoreConfig.Name << "(mps_in=" << Mps1 << ") < "
                                        << pComponent2->CoreConfig.Name << "(mps_in=" << Mps2 << ") = "
                                        << Result);
   return Result == Ascending;
}

bool DASHmpsoCompare(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column) {
   COL_FUNCTION(DASHmpsoCompare);
   float Mps1 = SDBIgetComponentStats(pComponent1->CoreConfig.Guid, false);
   float Mps2 = SDBIgetComponentStats(pComponent2->CoreConfig.Guid, false);
   if (Mps1 == Mps2) { return DASHnameCompare(pComponent1, pComponent2, Ascending, Column); }
   bool Result = Mps1 < Mps2;
   COL_TRC(pComponent1->CoreConfig.Name << "(mps_out=" << Mps1 << ") < "
                                        << pComponent2->CoreConfig.Name << "(mps_out=" << Mps2 << ") = "
                                        << Result);
   return Result == Ascending;
}

bool DASHnameCompare(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column) {
   COL_FUNCTION(DASHnameCompare);
   // IX-831 - Substitute in COLstring::icompare() for a comparison which doesn't put _ above A at the top.
   // bool Result = COLstring::icompare(pComponent1->CoreConfig.Name.c_str(), pComponent2->CoreConfig.Name.c_str()) >=
   // 0;
   const COLstring& Name1 = pComponent1->CoreConfig.Name;
   const COLstring& Name2 = pComponent2->CoreConfig.Name;
   const bool       Result =
       Name1 == Name2 ? DASHicompare(pComponent1->CoreConfig.Guid.c_str(), pComponent2->CoreConfig.Guid.c_str()) >= 0
                            : DASHicompare(pComponent1->CoreConfig.Name.c_str(), pComponent2->CoreConfig.Name.c_str()) >= 0;
   COL_TRC(pComponent1->CoreConfig.Name << " > " << pComponent2->CoreConfig.Name << " = " << Result);
   return Result == Ascending;
}

bool DASHlightCompare(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column) {
   COL_FUNCTION(DASHlightCompare);
   if(pComponent1->Status.Light == pComponent2->Status.Light) { return DASHnameCompare(pComponent1, pComponent2, Ascending, Column); }
   return (pComponent1->Status.Light < pComponent2->Status.Light) == Ascending;
}

bool DASHerrorCompare(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column) {
   COL_FUNCTION(DASHerrorCompare);
   if(pComponent1->Status.ErrorCount == pComponent2->Status.ErrorCount) {
      COL_TRC(pComponent1->CoreConfig.Name << " and " << pComponent1->CoreConfig.Name << " need name compare (equal error counts)");
      return DASHnameCompare(pComponent1, pComponent2, Ascending, Column);
   }
   bool Result = pComponent1->Status.ErrorCount < pComponent2->Status.ErrorCount;
   COL_TRC(pComponent1->CoreConfig.Name << "(err=" << pComponent1->Status.ErrorCount <<  ") > "
        << pComponent2->CoreConfig.Name << "(err=" << pComponent2->Status.ErrorCount <<  ") = " << Result);
   return Result == Ascending;
}

static bool DASHqueueComparePosition(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column){
   COL_FUNCTION(DASHqueueComparePosition);
   COLuint64 QueuePosition1 = DASHfindEarliestQueuePosition(pComponent1->CoreConfig.Guid);
   COLuint64 QueuePosition2 = DASHfindEarliestQueuePosition(pComponent2->CoreConfig.Guid);
   if(QueuePosition1 == QueuePosition2) {
      COL_TRC(pComponent1->CoreConfig.Name << " and " << pComponent1->CoreConfig.Name << " need name compare (equal error counts)");
      return DASHnameCompare(pComponent1, pComponent2, Ascending, Column);
   }
   bool Result = QueuePosition1 > QueuePosition2;
   COL_TRC(pComponent1->CoreConfig.Name << "(que_pos=" << QueuePosition1 <<  ") > "
      << pComponent2->CoreConfig.Name << "(que_pos=" << QueuePosition2 <<  ") = " << Result);
   return Result == Ascending;
}

bool DASHqueueCompare(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column) {
   COL_FUNCTION(DASHqueueCompare);
   COL_VAR2(*pComponent1, *pComponent2);
   int QueueCount1 = SDBIgetQueueCount(pComponent1->CoreConfig.Guid);
   int QueueCount2 = SDBIgetQueueCount(pComponent2->CoreConfig.Guid);
   if (QueueCount1 == QueueCount2){ // This could happen when both components are at end of queue (count = 0), in which case we need to look at queue position to see what the order should be
      return DASHqueueComparePosition(pComponent1, pComponent2, Ascending, Column);
   } 
   bool Result = QueueCount1 < QueueCount2;
   COL_TRC(pComponent1->CoreConfig.Name << "(que=" << QueueCount1 <<  ") < "
      << pComponent2->CoreConfig.Name << "(que=" << QueueCount2 <<  ") = " << Result);
   return Result == Ascending;
}

bool DASHserverCompare(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column) {
   COL_FUNCTION(DASHserverCompare);
   if(pComponent1->m_HostServerGuid == pComponent2->m_HostServerGuid) { return DASHnameCompare(pComponent1, pComponent2, Ascending, Column); }
   return (pComponent1->m_HostServerGuid > pComponent2->m_HostServerGuid) == Ascending;
}

bool DASHtryParseInt(const COLstring& S, int& Result) {
   COL_FUNCTION(DASHtryParseInt);
   if(S.is_null()) return false;
   for(int i = 0; i < S.size(); ++i) {
      if(!isdigit(S[i])) return false;
   }
   try {
      long num = atol(S.c_str());
      if(num < COL_INT_32_MIN || num > COL_INT_32_MAX) return false;
      Result = static_cast<int>(num);
      return true;
   } catch(const COLerror& E) {
      COL_TRC(E.description());
      return false;
   }
}

bool DASHcustomCompare(CFGconfig* pComponent1, CFGconfig* pComponent2, bool Ascending, COLstring Column) {
   COL_FUNCTION(DASHcustomCompare);
   COL_VAR(Column);
   COLvar Comp1Map, Comp2Map;
   ICOLcomponentGet(pComponent1->CoreConfig.Guid, &Comp1Map);
   ICOLcomponentGet(pComponent2->CoreConfig.Guid, &Comp2Map);
   COL_VAR2(Comp1Map.json(1), Comp2Map.json(1));
   const COLstring Val1 = Comp1Map.exists(Column) ? Comp1Map[Column] : "";
   const COLstring Val2 = Comp2Map.exists(Column) ? Comp2Map[Column] : "";
   COL_VAR2(Val1, Val2);
   if(Val1 == Val2) { return DASHnameCompare(pComponent1, pComponent2, Ascending, Column); }
   if(!Val1.is_null() && Val2.is_null()) { return !Ascending; }
   if(Val1.is_null() && !Val2.is_null()) { return Ascending; }
   int Val1Int, Val2Int;
   if(DASHtryParseInt(Val1, Val1Int) && DASHtryParseInt(Val2, Val2Int)) {
      return (Val1Int < Val2Int) == Ascending;
   }
   return (Val1 > Val2) == Ascending;
}

DASHcompareFunc DASHgetCompareFunction(const COLstring& Column){
   COL_FUNCTION(DASHgetCompareFunction);
   COLstring SortKey = Column;
   SortKey.toUpperCase();
   if(SortKey == "RUN")    { return &DASHlightCompare;  }
   if(SortKey == "ERR")    { return &DASHerrorCompare;  }
   if(SortKey == "QUE")    { return &DASHqueueCompare;  }
   if(SortKey == "SERVER") { return &DASHserverCompare; }
   if(SortKey == "NAME")   { return &DASHnameCompare;   }
   if(SortKey == "LAST")   { return &DASHlastCompare;   }
   if(SortKey == "IN")     { return &DASHmpsiCompare;   }
   if(SortKey == "OUT")    { return &DASHmpsoCompare;   }
   if(SortKey == "ACT")    { return &DASHnameCompare;   } // TODO - make one for this
   COL_TRC("Warning - unknown comparison -- attempting to look through custom column values");
   return &DASHcustomCompare;
}
