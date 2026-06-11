#ifndef __PGS_SERVER_H__
#define __PGS_SERVER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: PGSserver
//
// Author: Nicolas Lehman
//
// Description:
//
// Facilitates management of bundled postgres database.
//---------------------------------------------------------------------------

class COLstring;
#include <COL/COLostream.h>

enum PGSstatusCode {
   PGS_OK,
   PGS_INITDB_OK,
   PGS_INITDB_ALREADY_EXISTS,
   PGS_ZOMBIE, // TODO - choose a better name for this
   PGS_DB_NOT_READY,
   PGS_DB_NO_RESPONSE,
   PGS_BIN_NOT_FOUND,
   PGS_BIN_NOT_RUNNABLE,
   PGS_ERR
};

// triggers the initdb routine to initialize the postgres database
// returns true it if succeeded and false if not.
bool PGSinitalizeDatabase (const COLstring& DataDir, const COLstring& BinDir, const COLstring& Password, COLstring* pInitDbOutput, int* pExitCode);

// Return true if the directory is empty otherwise it will be false.
bool PGSdoesDatabaseNeedInitialization(const COLstring& DataDir);

// Implements heuristic to look at InitDbOutput and guess if it showing signs of being a Parallels data dir.
bool PGSdoesOutputLookLikeParallelsDrive(const COLstring& InitDbOutput);

bool PGSparsePostgresStatus (PGSstatusCode Status, const COLstring& BinDir, COLostream& LogOutput);

// Checks for postgres running on specific database directory
bool PGSisPostgresRunning(const COLstring& DataDir, const COLstring& BinDir);

// Gracefully stop postgres
bool PGSsmartStop(const COLstring& DataDir, const COLstring& BinDir, COLstring* pOutput, int* pExitCode);

// Forcefully stop postgress
bool PGSforceStop(const COLstring& DataDir, const COLstring& BinDir, COLstring* pOutput, int*ExitCode);

// Start postgres - does not wait to see if is fully done.
bool PGSstart (const COLstring& DataDir, const COLstring& BinDir, COLstring* pOutput, int* pExitCode, bool EnableLog = true);

// Is the database ready for use.
bool PGSisReady(const COLstring& DataDir, const COLstring& BinDir);

#endif // End of defensive include.
