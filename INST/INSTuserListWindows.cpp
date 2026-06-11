// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTuserList
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Sunday 16 July 2023 - 04:34PM
// ---------------------------------------------------------------------------

#include <INST/INSTuserList.h>

#include <COL/COLwindows.h>

#include <lmcons.h>
#include <lm.h>

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool INSTuserIgnore(const COLstring& User){
   COL_FUNCTION(INSTuserIgnore);
   if (User == "DefaultAccount" ||
       User == "Administrator" ||
       User == "Guest" ||
       User == "WDAGUtilityAccount"){
      COL_TRC("Ignore " << User);
      return true;
   }
   return false;
}


void INSTuserList(COLarray<COLstring>* pList) {
   COL_FUNCTION(INSTuserList);
   USER_INFO_0* userInfo = NULL;
   DWORD entriesRead = 0;
   DWORD totalEntries = 0;
   // Retrieve user information
   NET_API_STATUS result = NetUserEnum(NULL, 0, FILTER_NORMAL_ACCOUNT, (LPBYTE*)&userInfo, MAX_PREFERRED_LENGTH, &entriesRead, &totalEntries, NULL);
   if (result == NERR_Success) {
      for (DWORD i = 0; i < entriesRead; i++) {
         COLstring User = userInfo[i].usri0_name;
         COL_VAR(User);
         if (!INSTuserIgnore(User)){
            pList->push_back(User);
         }
      }
   }

   // Free memory allocated for user information
   if (userInfo != NULL) {
      NetApiBufferFree(userInfo);
   }
}
