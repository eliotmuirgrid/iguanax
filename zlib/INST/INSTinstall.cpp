// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTinstall
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sunday 13 August 2023 - 07:12AM
// ---------------------------------------------------------------------------

#include <INST/INSTinstall.h>

#include <SERV/SERVdescription.h>
#include <SERV/SERVdisplayName.h>

#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// SINGLE HELPER FUNCTION so we are consistent
// TODO - IX-1933
bool INSTinstall(const COLstring& Exe, const COLstring& User, const COLstring& Password, COLstring* pError){
   COL_FUNCTION(INSTinstall);
   return SDMserverInstall(SDMserviceName(), SERVdisplayName(), SERVdescription(), Exe, User, Password, true, pError);
}