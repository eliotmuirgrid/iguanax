//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UNITlastFailed
//
// Author: Eliot Muir
//
// Date: Wed 17 Mar 2021 13:18:26 EDT
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include "UNITlastFailed.h"
#include "UNITrun.h"

#include <FIL/FILutils.h>

#include <COL/COLhashmap.h>
#include <COL/COLlist.h>
#include <COL/COLsplit.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void UNITsaveFailedTests(const UNITsummary& Summary){
   COL_FUNCTION(UNITsaveFailedTests);
   COLstring Output;
   for (COLmap<COLstring, COLstring>::const_iterator i = Summary.m_FailedBuilds.cbegin(); i != Summary.m_FailedBuilds.cend(); i++){
      if (i->second != "succeeded"){
         Output += i->first + "\n";
      }
   }
   if (!Output.is_null()){
      FILwriteFile("lastfailed.tmp", Output);
      COLcout << "Wrote failed tests into lastfailed.tmp - to rerun failed tests use --lastfailed flag." << newline;
   } else {
      if (FILfileExists("lastfailed.tmp")){
         FILremoveWithThrow("lastfailed.tmp");
      }
   }
}


void UNITfilterFailedTests(COLmap<COLstring, COLauto<COLclosure0> >* pTests){
   COL_FUNCTION(UNITfilterFailedTests);
   if (!FILfileExists("lastfailed.tmp")){
      COLcout << "No failed tests to run." << newline;
      pTests->clear();
      return;
   }
   COLstring Content;
   FILreadFile("lastfailed.tmp", &Content);
   COLarray<COLstring> List;
   COLsplit(&List, Content, "\n");
   COLhashmap<COLstring, COLstring> Map;
   for (int i=0; i < List.size(); i++){
      Map.add(List[i].strip(COLstring::Both, ' '), "");
   }
   COLlist<COLstring> RemoveList;
   for (auto i = pTests->cbegin(); i != pTests->cend(); i++){
      if (!Map.find(i->first)){
          RemoveList.push_back(i->first);
          COL_TRC("Remove " << i->first);
      }
   } 
   for (COLlist<COLstring>::const_iterator j = RemoveList.cbegin(); j != RemoveList.cend(); j++){
      pTests->removeKey(*j);
   }
}
