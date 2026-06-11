//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiDisconnect.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GRID/GRIDnode.h>
#include <GRIDA/GRIDAdisconnect.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

// /iguanas/disconnect
void GRIDAdisconnect(const COLwebRequest& Request, GRIDnode* pNode) {
   COL_FUNCTION(GRIDAdisconnect);
   COL_VAR(Request.Data);
   COLstring Error;
   if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", EDIT_PEERS, &Error)) {
      return COLrespondError(Request.Address, PRMsetError(EDIT_PEERS));
   }
   if(!pNode) { return COLrespondError(Request.Address, "Grid not initialized."); }
   const COLstring IguanaId = COLvarRequiredString(Request.Data, "id");
   pNode->disconnect(IguanaId);
   COLrespondSuccess(Request.Address);
}