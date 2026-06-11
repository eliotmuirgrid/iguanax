#ifndef __SERV_INSTALL_H__
#define __SERV_INSTALL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVinstall
//
// Description:
//
// Installation routine for --service install
//
// Author: Tyler Brown 
// Date:   Monday 21 August 2023 - 02:53PM
// ---------------------------------------------------------------------------

class COLstring;

int SERVinstall(const COLstring& Username, const COLstring& Password);

#endif // end of defensive include
