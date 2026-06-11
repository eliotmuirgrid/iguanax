#ifndef __IWEB_LOAD_H__
#define __IWEB_LOAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: IWEBload
//
// Description:
//
// Loading routine for IWEBconfig
//
// Author: Eliot Muir 
// Date:   Sunday 18 September 2022 - 06:29PM
// ---------------------------------------------------------------------------

class IWEBconfig;
class COLstring;

bool IWEBcreateDefaultCertKey();


// Returns false if it fails.
bool IWEBload(IWEBconfig* pConfig, COLstring* pError);

#endif // end of defensive include
