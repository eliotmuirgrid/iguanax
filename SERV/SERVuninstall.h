#ifndef __SERV_UNINSTALL_H__
#define __SERV_UNINSTALL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVuninstall
//
// Description:
//
// --service unintall routine.  Also has a stop with retry routine which is
// used from --service restart.
//
// Author: Tyler Brown 
// Date:   Monday 21 August 2023 - 02:56PM
// ---------------------------------------------------------------------------

int SERVuninstall();

bool SERVstopServerWithRetry();

#endif // end of defensive include
