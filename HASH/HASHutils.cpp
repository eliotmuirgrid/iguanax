//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HASHutils.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   05/04/24
//  ---------------------------------------------------------------------------
#include <HASH/HASHutils.h>

#include <COL/COLlog.h>
#include <COL/COLsplit.h>
#include <COL/COLvar.h>
COL_LOG_MODULE;

COLstring HASHparseError() {
   return "Trouble parsing hashtags - it must either be a string or an array";
}

COLstring HASHerror() {
   // we used to allow '#', but we don't want to "advertise" it any more
   return "Hashtags must start with '#', be at least two characters long and only contain alphanumeric characters or "
          "'_'.";
}

bool HASHisValid(const COLstring& Tag) {
   COL_FUNCTION(HASHisValid);
   COL_VAR(Tag);
   if(Tag.size() <= 1) { return false; }
   if(Tag[0] != '#') { return false; }
   for(int i = 1; i < Tag.size(); ++i) {
      const char c = Tag[i];
      // we allow '#' for backward compatibility reason
      if(!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '#')) {
         return false;
      }
   }
   return true;
}

void HASHstringToArr(const COLstring& HashString, COLarray<COLstring>* pArr) {
   COL_FUNCTION(HASHstringToArr);
   COLarray<COLstring> TagVec;
   COLsplit(&TagVec, HashString, " ");
   for(const auto& it : TagVec) {
      if(HASHisValid(it)) { pArr->push_back(it); }
   }
}

bool HASHvarToArr(const COLvar& HashVar, COLarray<COLstring>* pArr) {
   COL_FUNCTION(HASHvarToArr);
   if(HashVar.isString()) {
      COL_TRC("Hash to parse is a string");
      HASHstringToArr(HashVar.asString(), pArr);
      return true;
   }
   if(!HashVar.isArray()) { return false; }
   for(const auto& it : HashVar.array()) {
      if(it.asString().isWhitespace() || it.asString().is_null()) { continue; }
      COL_VAR(it);
      if(HASHisValid(it)) { pArr->push_back(it); }
   }
   return true;
}

COLstring HASHarrToString(const COLarray<COLstring>& HashArr) {
   COL_FUNCTION(HASHarrToString);
   COLstring Res;
   for(const auto& it : HashArr) { Res.append(it + " "); }
   Res = Res.strip(COLstring::StripType::Trailing, ' ');
   return Res;
}

void HASHstringToVar(const COLstring& HashString, COLvar* pHashVar) {
   COL_FUNCTION(HASHstringToVar);
   COLarray<COLstring> Arr;
   HASHvarToArr(HashString, &Arr);
   pHashVar->setArrayType();
   for(const auto& it : Arr) { pHashVar->push_back(it); }
}
