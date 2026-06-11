//---------------------------------------------------------------------------
// Copyright (C) 1997-2017 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GUIDid
//
// Description:
//
// Implementation
//
// Author:  Akshay Ganeshen
// Date:    Wed Mar 25 10:13:36 EDT 2015
//---------------------------------------------------------------------------

#include "GUIDid.h"

#include <COL/COLstring.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <GUID/GUIDutils.h>

COLstring GUIDnewId(int Length) {
   COL_FUNCTION(GUIDnewId);
   COL_VAR(Length);

   COLstring Result;
   while (Result.size() < Length) {
      COLstring RawBytes = GUIDentropyString();
      Result.append(GUIDencodeBase62(RawBytes));
      COL_VAR(Result);
   }
   if (Result.size() != Length) {
      Result = Result.substr(0, Length);
   }
   return Result;
}

COLstring GUIDnewId(){
   COL_FUNCTION(GUIDnewId);
   return GUIDnewId(22);
}

COLstring GUIDnewShortId() {
   COL_FUNCTION(GUIDnewShortId);
   return GUIDnewId(14);
}

COLstring GUIDnewTinyId() {
   COL_FUNCTION(GUIDnewTinyId);
   return GUIDnewId(8);
}
