//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UNITfilter
//
// Author: Eliot Muir
//
// Date: Wed 17 Mar 2021 13:18:26 EDT
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------
#include "UNITfilter.h"

#include <COL/COLutils.h>  // for COLglobMatch function
#include <COL/COLlist.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void UNITfilterTests(const COLstring& Glob, COLmap<COLstring, COLauto<COLclosure0> >* pTests){
   COL_FUNCTION(UNITfilterTests);
   COL_VAR(Glob);
   COLlist<COLstring> RemoveList;
   for (auto i = pTests->cbegin(); i != pTests->cend(); i++){
      if (!COLglobMatch(Glob.c_str(), i->first.c_str()
      )){
          RemoveList.push_back(i->first);
          COL_TRC("Remove " << i->first);
      }
   } 
   for (COLlist<COLstring>::const_iterator j = RemoveList.cbegin(); j != RemoveList.cend(); j++){
      pTests->removeKey(*j);
   }
}
