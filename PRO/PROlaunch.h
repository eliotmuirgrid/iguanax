#ifndef __PRO_LAUNCH_H__
#define __PRO_LAUNCH_H__
//------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  PROlaunch
// 
// Description
//
// Launch a process
// 
// Author: Shuo Hou
// Date:   June 5 2021
//---------------------------------------------------------------------------

class COLstring;

int PROlaunch(const COLstring& AppDir, const COLstring& FullCommand);

#endif