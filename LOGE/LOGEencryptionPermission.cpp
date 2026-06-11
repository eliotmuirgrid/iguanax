// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGEencryptionPermission
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 11 November 2024 - 03:03PM
// ---------------------------------------------------------------------------

#include <LOGE/LOGEencryptionPermission.h>
#include <PRM/PRMcallbacks.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// /log/encryption/permission
void LOGEencryptionPermission(const COLwebRequest& Request){
   COL_FUNCTION(LOGEencryptionPermission);
   COLstring User = Request.User;
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, User, "", ROLid::EDIT_LOG_ENCRYPTION, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_LOG_ENCRYPTION));
   }
   COLrespondSuccess(Request.Address);
} 
  