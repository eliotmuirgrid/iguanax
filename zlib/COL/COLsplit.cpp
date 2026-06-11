//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLsplit
//
// Description:
//
// Implementation
//
// Author: Dmitri Klimenko
// Date:   Monday, January 25th, 2010 @ 11:17:51 AM
//
//---------------------------------------------------------------------------
#include "COLsplit.h"
#include <ctype.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void COLsplit(COLarray<COLstring>* pOutPartList, const COLstring& String, const COLstring& Separator){
   COL_FUNCTION(COLsplit);
   COL_VAR2(String, Separator);
   COLarray<COLstring>& OutPartList = *pOutPartList;
   COLASSERT(!Separator.is_null()); // can't split by an empty string - it just doesn't make sense
   if (String.is_null()) {
      // we return zero parts for an empty string - then it's easy to distinguish 2 cases:
      // if we get zero parts, we know it was empty
      // if we get nonzero parts, we know it was NOT empty
      return;
   }
   COLstring Remainder = String;
   COLstring Head, Tail;
   while(Remainder.split(Head, Tail, Separator.c_str())){
      OutPartList.push_back() = Head;
      Remainder = Tail;
   }
   OutPartList.push_back() = Remainder;
   COL_VAR(OutPartList.size());
}

void COLsplit(COLvar* pOutPartList, const COLstring& String, const COLstring& Separator){
   COL_FUNCTION(COLsplit);
   COL_VAR2(String, Separator);
   COLASSERT(!Separator.is_null());
   COLvar& OutPartList = *pOutPartList;
   OutPartList.setArrayType(); // make an empty array if there are no items in the list - Eliot
   if (String.is_null()) { return; }
   COLstring Remainder = String;
   COLstring Head, Tail;
   while (Remainder.split(Head, Tail, Separator.c_str())){
      OutPartList.push_back() = Head;
      Remainder = Tail;
   }
   OutPartList.push_back() = Remainder;
   COL_VAR(OutPartList);
}

void COLjoin(COLstring* pJoined, const COLarray<COLstring>& PartList, const COLstring& Separator, bool Quote) {
   COL_FUNCTION(COLjoin);
   COLstring& Joined = *pJoined;
   for (int i = 0; i < PartList.size(); i++) {
      if (i > 0) { 
         Joined.append(Separator);
      }
      if (Quote) { Joined.append ("\""); }
      Joined.append(PartList[i]);
      if (Quote) { Joined.append ("\""); }
   }
   COL_VAR3(PartList.size(), Quote, Joined);
}

void COLjoin(COLstring* pJoined, const COLvar& PartList, const COLstring& Separator, bool Quote) {
   COL_FUNCTION(COLjoin);
   COLstring& Joined = *pJoined;
   bool FinishedTheFirst = false;
   COL_VAR(PartList.type());
   if (! PartList.isArray()) {return;};
   const COLvar::COLvarVec& TheList = PartList.array();
   for (COLvar::COLvarVec::const_iterator i = TheList.begin(); i != TheList.end(); ++i) {
      if (! i->isString()) {continue;}
      if (FinishedTheFirst) { 
         Joined.append(Separator);
      }
      if (Quote) { Joined.append ("\""); }
      Joined.append(i->asString());
      if (Quote) { Joined.append ("\""); }
      FinishedTheFirst = true;
   }
   COL_VAR3(PartList, Quote, Joined);
}

COLindex COLsplitWhiteSpace(COLarray<COLstring>* pOutPartList, const COLstring& Source, bool KeepSpace) {
   COL_FUNCTION(COLsplitWhiteSpace);
   COLarray<COLstring>& OutPartList = *pOutPartList;
   COLstring Space;
   COLstring NonSpace;
   COLindex PartCount=0;
   
   for (COLindex SourceIndex=0;;) {
      if (SourceIndex >= Source.size()) {
         break;
      }
      
      if (COLisspace(Source[SourceIndex])) {
         if (!NonSpace.is_null()) {
            OutPartList.push_back() = NonSpace;
            PartCount++;
            NonSpace = "";
         }
         if (KeepSpace)Space += Source[SourceIndex];
         SourceIndex++;
         continue;
      }
      
      // nonspace
      if (KeepSpace && !Space.is_null()) {
         OutPartList.push_back() = Space;
         PartCount++;
         Space = "";
      }
      NonSpace += Source[SourceIndex];
      SourceIndex++;
   };
   if (!NonSpace.is_null()) {
      OutPartList.push_back() = NonSpace;
      PartCount++;
      NonSpace = "";
   };
   if (KeepSpace && !Space.is_null()) {
      OutPartList.push_back() = Space;
      PartCount++;
      Space = "";
   }
   return PartCount;
};

