#ifndef __DBD_PRE_START_H__
#define __DBD_PRE_START_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDpreStart
//
// Description:
//
// Misnamed - start up routines for Iguana.
//
// Author: Eliot Muir
// Date:   Friday 23 September 2022 - 06:45PM
// ---------------------------------------------------------------------------

class CMDlineParser;
class COLstring;
class DBDworld;

// Sets iguana working dir to
//  - command line --working_dir argument if present, or
//  - log_dir value in iguana.ini file if specified, or
//  - the default working dir
// Returns
//  - true if working is created and set
//  - false otherwise, and pError is populated with error message
// Assumes
//  - iguana.ini file is already loaded using SERVloadServiceConfiguration()
bool DBDsetWorkingDir(const CMDlineParser& Parser, COLstring* pError);

// Prepares the working dir
// Assumes DBDsetWorkingDir() is called earlier
void DBDprepWorkingDir(const COLstring& LoadedWorkingDir, bool NoMigration);
void DBDgetLinuxSudoUser(COLstring* pUsername);
void DBDsetPermissions(const COLstring& Username, const COLstring& WorkingDir);

void DBDinstall(DBDworld* pWorld, int argc, const char** argv);
int DBDrun(DBDworld* pWorld);

#endif // end of defensive include
