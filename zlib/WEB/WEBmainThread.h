#ifndef __WEB_MAIN_THREAD_H__
#define __WEB_MAIN_THREAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: WEBmainThread
//
// Description:
//
// Functions to set and determine whether we are on the main thread, used by WEBresponse.
//
// Author: Vismay Shah 
// Date:   Wednesday 24 May 2023 - 01:05PM
// ---------------------------------------------------------------------------
#include <COL/COLthread.h>

void WEBsetMainThread();
bool WEBisMainThread(COLthreadID CurrentThread);

#endif // end of defensive include
