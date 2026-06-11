#ifndef __INST_WORKING_DIR_H__
#define __INST_WORKING_DIR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTworkingDir
//
// Description:
//
// APIs that allows users to set a custom IguanaX working dir.
//
// Author: Tyler Brown 
// Date:   Tuesday 08 August 2023 - 01:17PM
// ---------------------------------------------------------------------------

class COLstring;
class COLwebRequest;

void INSTworkingDir(const COLwebRequest& Request);

void INSTsaveWorkingDir(const COLstring& IniFile, const COLstring& WorkingDir);
COLstring INSTgetWorkingDir(const COLstring& ExePath);

#endif // end of defensive include
