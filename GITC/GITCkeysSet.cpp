// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITCkeysSet
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 04 December 2025 - 01:41PM
// ---------------------------------------------------------------------------
#include <GITC/GITCkeysSet.h>

#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GITU/GITUconfigGlobal.h>
#include <GITU/GITUcopyOldKey.h>

#include <SCK/SCKloop.h>

#include <COL/COLthreadPool.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void GITCkeysSetImpl(COLstring PrivateKeyData, COLstring PublicKeyData, COLstring User, COLaddress Address){
   COL_FUNCTION(GITCkeysSetImpl);
   const COLstring PrivateKeyPath = DIRconfigUserSshFile(User);
   COL_VAR(PrivateKeyPath);
   COLstring Out;
   COL_TRC("Write out private key");
   if (!GITUwritePrivateKey(PrivateKeyPath, PrivateKeyData, &Out)) {
      return COLrespondError(Address, Out);
   }
   COL_TRC("Write out public key");
   if (!GITUwritePublicKey(PrivateKeyPath, PublicKeyData, &Out)){
      return COLrespondError(Address, Out);
   }
   return COLrespondSuccess(Address);
}

// /git/keys/set
void GITCkeysSet(const COLwebRequest& Request, COLmap<COLstring, USRuser>* pUserMap, SCKloop* pLoop) {
   COL_FUNCTION(GITCkeysSet);
   COLstring PrivateKey = COLvarRequiredString(Request.Data, "private_key");
   COLstring PublicKey  = COLvarRequiredString(Request.Data, "public_key");
   // Make sure user already exists. This prevents garbage component guids from being created in the config/users dir
   if (pUserMap->count(Request.User) == 0) {
      return COLrespondError(Request.Address, "Cannot set keys. User '" + Request.User + "' does not exist.");
   }
   // COL_TRC("Cleaning up input keys");
   PrivateKey.replace("\r\n", "\n");
   PublicKey.replace("\r\n", "\n");
   // Run file writes on worker
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&GITCkeysSetImpl,PrivateKey, PublicKey, Request.User, Request.Address));
}