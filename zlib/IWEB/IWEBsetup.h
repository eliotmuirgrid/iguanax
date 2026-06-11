#ifndef __IWEB_SETUP_H__
#define __IWEB_SETUP_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBsetup
//
// Description:
//
// Create routines for IWEBconfig
//
// Author: Eliot Muir 
// Date:   Monday 19 September 2022 - 01:49PM
// ---------------------------------------------------------------------------
class IWEBconfig;

void IWEBcreate (IWEBconfig** ppConfig);
void IWEBdestroy(IWEBconfig** ppConfig);

#endif // end of defensive include
