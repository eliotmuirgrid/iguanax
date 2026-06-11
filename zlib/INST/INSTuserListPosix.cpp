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

#include <pwd.h>

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void INSTuserList(COLarray<COLstring>* pList) {
   COL_FUNCTION(INSTuserList);
   struct passwd *user;
   while ((user = getpwent()) != NULL) {
      if (user->pw_name[0] != '_'){
         pList->push_back(user->pw_name);
      }
   }
   endpwent();
}