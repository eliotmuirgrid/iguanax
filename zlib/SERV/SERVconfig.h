#ifndef __SERV_CONFIG_H__
#define __SERV_CONFIG_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVconfig
//
// Description:
//
// Loads config file that specifies service details if it can find it.
//
// Author: Tyler Brown 
// Date:   Wednesday 23 August 2023 - 11:39AM
// ---------------------------------------------------------------------------

class COLstring;

void SERVloadServiceConfiguration(COLstring* pFileName);

#endif // end of defensive include
