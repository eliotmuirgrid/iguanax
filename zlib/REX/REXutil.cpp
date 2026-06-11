//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: REXutil
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday, December 21st, 2012 @ 09:13:53 AM
//
//---------------------------------------------------------------------------
#include "REXutil.h"
#include "REXfind.h"

#include <COL/COLstring.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring REXreplace(const COLstring& Input, const char* Regex, const COLstring& With, bool CaseSensitive){
   COLstring Out;
   REXfind Pattern;
   
   if (CaseSensitive) {
      Pattern.init(Regex, REXfind::REX_NOTEMPTY);
   } else {
      Pattern.init(Regex, REXfind::REX_NOTEMPTY | REXfind::REX_CASE_INSENSITIVE);
   }

   Pattern.matchBegin(Input.c_str(), Input.size());
   while (Pattern.matchNext()){
      Out.append(Pattern.unmatched().pData,Pattern.unmatched().Size);
      Out.append(With);
   }
   Out.append(Pattern.unmatched().pData,Pattern.unmatched().Size);
   return Out;
}

bool REXmatches(const COLstring& Input, const char* Regex){
   REXfind Pattern;
   Pattern.init(Regex);
   Pattern.matchBegin(Input.c_str(), Input.size());
   return Pattern.matchNext();
}
