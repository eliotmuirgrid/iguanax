//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HAtypes.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <HA/HAtypes.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

HAtype HAstringToType(const COLstring& Type) {
   COL_FUNCTION(HAstringToType);
   if(Type == "Active-Passive Shared Drive") { return ACTIVE_PASSIVE_SHARED_DRIVE; }
   return NONE;
}
COLstring HAtypeToString(const HAtype& Type) {
   COL_FUNCTION(HAtypeToString);
   switch(Type) {
      case ACTIVE_PASSIVE_SHARED_DRIVE: return "Active-Passive Shared Drive";
      default:                          return "";
   }
}