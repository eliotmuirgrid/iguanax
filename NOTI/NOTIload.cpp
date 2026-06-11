// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: NOTIload.cpp
//
//  Description: Notification config api handler
//
//  Author: Matthew Sobkowski
//  Date:   8/31/22, 10:53 AM
// ---------------------------------------------------------------------------
#include "NOTIload.h"

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <NOTI/NOTImodel.h>
COL_LOG_MODULE;

// /notifications
void NOTIload(const COLwebRequest& Request, NOTImodel* pNoti) {
   COL_FUNCTION(NOTIload);
   COLvar Response;
   NOTImodelToWebVar(&Response, *pNoti);
   COLrespondSuccess(Request.Address, Response);
}