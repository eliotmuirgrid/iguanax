#ifndef __SERV_WORKING_DIR_H__
#define __SERV_WORKING_DIR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVworkingDir
//
// Description:
//
// Set the working directory of the service.
//
// Author: Tyler Brown 
// Date:   Wednesday 23 August 2023 - 03:15PM
// ---------------------------------------------------------------------------

class COLstring;

void SERVsetWorkingDir(const COLstring& WorkingDir);

COLstring SERVworkingDir();

#endif // end of defensive include
