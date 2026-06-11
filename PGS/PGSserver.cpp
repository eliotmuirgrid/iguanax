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

#include "PGSserver.h"

#include <COL/COLdll.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
#include <COL/COLutils.h>
COL_LOG_MODULE;

#include <SCK/SCKloop.h>
#include <FIL/FILfile.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <PRO/PROexecute.h>
#include <PRO/PROenv.h>

#include <signal.h>

#define PGS_MAX_ATTEMPTS 10

static const COLstring PG_PASS_FILE = ".pgpass";

static COLstring PasswordFilePath() {
   COLstring Path;
#ifdef _WIN32
   Path = PROgetenv("TEMP");
#endif
#ifdef __APPLE__
   Path = PROgetenv("TMPDIR");
#endif
#ifdef __linux__
   Path = "/tmp";
#endif
   FILaddPathSeparator(Path);
   return Path + PG_PASS_FILE;
}

static COLstring PathEscape(const COLstring& Path) {
   COL_FUNCTION(PathEscape);
   COL_VAR(Path);
   COLstring NewPath = Path;
   NewPath.prepend("\"");
#ifndef _WIN32
   NewPath.replace(" ", "\\ ");
#endif
   NewPath += "\"";
   COL_VAR(NewPath);
   return NewPath;
}

#ifdef _WIN32
// Prevents Postgres from inheriting Fleet Manager's sockets and port, which causes web server restart problems
static int PGSspawnWindowsPostgresProcess(const COLstring& BinDir, const COLstring& Cmd, COLstring* pFeedback) {
   COL_FUNCTION(PGSspawnWindowsPostgresProcess);

   int ExitCode = -1;
   bool Success = false;
   COLstring StorePath = PROgetenv("PATH");
   PROsetenv("PATH", BinDir); // Replace PATH with path to postgres binaries
   try{
      COLstring Out;
      ExitCode = PROexecuteString(".", BinDir + Cmd, &Out, &Out);
   } catch (const COLerror& Error) {
     COL_ERR(Error);
     *pFeedback = Error.description();
   }
   PROsetenv("PATH", StorePath); // Restore PATH

   if (PRO_TIMED_OUT == ExitCode) { *pFeedback = "Command timed out."; }
   COL_VAR(ExitCode);
   return ExitCode;
}
#endif

static int PGSexecuteDbCommand (const COLstring& BinDir, const COLstring& Cmd, COLstring* pFeedback) {
   COL_FUNCTION(PGSexecuteDbCommand);
   COL_VAR(Cmd);
   int ExitCode;
   COLstring Sink;
   COLstring StorePath = PROgetenv("PATH");
   PROsetenv("PATH", BinDir); // Replace PATH with path to postgres binaries
   try{
#ifdef _WIN32
   ExitCode = PROexecuteString(".", BinDir + Cmd, &Sink, &Sink);
#else
   ExitCode = PROexecuteString(".", Cmd, &Sink, &Sink);
#endif
   } catch (COLerror& Error){
     *pFeedback = Error.description();
     COL_ERR(Error);
     return 126;
   }
   PROsetenv("PATH", StorePath); // Restore PATH
   COLstring& ExitDetails = *pFeedback;
   ExitDetails = Sink;
   COL_VAR2(ExitCode, ExitDetails);
   return ExitCode;
}

static PGSstatusCode PGSerrorHandler (const int ExitCode, const COLstring& Result) {
   if       (127 == ExitCode)   { return PGS_BIN_NOT_FOUND; }
   else if  (126 == ExitCode)   { return PGS_BIN_NOT_RUNNABLE; }
   else {
      COL_ERR("Unexpected error: ExitCode = " << ExitCode << " Result = " << Result);
      return PGS_ERR;
   }
}

static PGSstatusCode PGSstatus (const COLstring& DataDir, const COLstring& BinDir) {
   COL_FUNCTION(PGSstatus);
   COLstring InitDatabase = "pg_ctl status -D " + PathEscape(DataDir);
   COLstring Result;
   int ExitCode = PGSexecuteDbCommand(BinDir, InitDatabase, &Result);
   if      (0 == ExitCode)       { return PGS_ZOMBIE; }
   else if (3 == ExitCode)       { return PGS_OK; }
   else if (4 == ExitCode)       { return PGS_OK; }
   else                          { return PGSerrorHandler(ExitCode, Result); }
}

bool PGSisReady(const COLstring& DataDir, const COLstring& BinDir) {
   COL_FUNCTION(PGSisReady);
   COLstring WaitForDb = "pg_isready";
#ifndef _WIN32
   WaitForDb += " -h " + PathEscape(DataDir);
#endif
   COLstring Result;
   int ExitCode = PGSexecuteDbCommand(BinDir, WaitForDb, &Result);
   return 0 == ExitCode;
}

bool PGSstart(const COLstring& DataDir, const COLstring& BinDir, COLstring* pOutput, int* pExitCode, bool EnableLog) {
   COL_FUNCTION(PGSstart);
   COLstring LogFilePath = FILpathAppend(DataDir, COL_T("..") + FIL_DIR_SEPARATOR + "postgres.log");
   LogFilePath = FILpathResolveEnvironment(LogFilePath);

   COLstring StartDatabase = "pg_ctl start --timeout=5 ";   // Try to start postgres in 5 seconds
#ifdef _WIN32
   if ( ! EnableLog ) { LogFilePath = "NUL"; }
   StartDatabase += " --log=" + PathEscape(LogFilePath); // Specify log option for pg_ctl
   StartDatabase += " -D " + PathEscape(DataDir);        // Set the database directory
   *pExitCode = PGSspawnWindowsPostgresProcess(BinDir, StartDatabase, pOutput);
#else
   if ( ! EnableLog ) { LogFilePath = "/dev/null"; }
   StartDatabase += " --log=" + PathEscape(LogFilePath);          // Specify log option for pg_ctl
   StartDatabase += " -D " + PathEscape(DataDir);                 // Set the database directory
   StartDatabase += " --options=\"";                              // Start the postgres options string
   StartDatabase += "-h \\\"\\\" ";                               // Set host to "" disabling listening on tcp/ip
   StartDatabase += "-k \\" + PathEscape(DataDir + "\\") + "\"";  // Set the unix socket directory
   *pExitCode = PGSexecuteDbCommand(BinDir, StartDatabase, pOutput);
#endif

   return (0 == *pExitCode);
}

bool PGSsmartStop (const COLstring& DataDir, const COLstring& BinDir, COLstring* pOutput, int* pExitCode) {
   COL_FUNCTION(PGSsmartStop);
   COLstring StopDb = "pg_ctl stop --timeout=5 --mode=smart -D " + PathEscape(DataDir);
   *pExitCode = PGSexecuteDbCommand(BinDir, StopDb, pOutput);
   return (0 == *pExitCode);
}

bool PGSforceStop (const COLstring& DataDir, const COLstring& BinDir, COLstring* pOutput, int* pExitCode) {
   COL_FUNCTION(PGSforceStop);
   COLstring StopDb = "pg_ctl stop --timeout=5 --mode=fast -D " + PathEscape(DataDir);
   *pExitCode = PGSexecuteDbCommand(BinDir, StopDb, pOutput);
   return (0 == *pExitCode);
}

static void setupPgPassFile(const COLstring& Password, const COLstring& BinDir) {
   COL_FUNCTION(setupPgPassFile);
   FILwriteFile(PasswordFilePath(), Password);
#ifndef _WIN32
   FILsetPermission(PasswordFilePath(), FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
#endif
}

static void teardownPgPassFile(const COLstring& BinDir) {
   COL_FUNCTION(teardownPgPassFile);
   FILremoveWithThrow(PasswordFilePath(), true);
}

bool PGSdoesDatabaseNeedInitialization(const COLstring& DataDir){
   COL_FUNCTION(PGSdoesDatabaseNeedInitialization);
   COL_VAR(DataDir);
   return FILisDirectoryEmpty(DataDir);
}

bool PGSinitalizeDatabase (const COLstring& DataDir, const COLstring& BinDir, const COLstring& Password, COLstring* pInitDbOutput, int* pExitCode) {
   COL_FUNCTION(PGSinitalizeDatabase);
   bool Success = false;
   setupPgPassFile(Password, BinDir);
   COLstring InitDatabase =
      "initdb"
      " --auth=md5"
      " --username=iguana"
      " --pwfile=" + PasswordFilePath() +
      " --pgdata=" + DataDir;
   *pExitCode = PGSexecuteDbCommand(BinDir, InitDatabase, pInitDbOutput);
   Success = ( *pExitCode == 0 || *pExitCode == 127);
   teardownPgPassFile(BinDir);
   COL_VAR(Success);
   return Success;
}

bool PGSdoesOutputLookLikeParallelsDrive(const COLstring& Result){
   COL_FUNCTION(PGSdoesOutputLookLikeParallelsDrive);
   return Result.find("could not link file \"pg_wal") != npos;
}

bool PGSisPostgresRunning(const COLstring& DataDir, const COLstring& BinDir){
   COL_FUNCTION(PGSisPostgresRunning);
   COLstring InitDatabase = "pg_ctl status -D " + PathEscape(DataDir);
   COLstring Result;
   int ExitCode = PGSexecuteDbCommand(BinDir, InitDatabase, &Result);
   return (ExitCode == 0);
}

// TODO - can we get rid of this?
bool PGSparsePostgresStatus (PGSstatusCode Status, const COLstring& BinDir, COLostream& LogOutput) {
   COL_FUNCTION(PGSparsePostgresStatus);
   bool Success = false;
   switch(Status) {
      case PGS_OK:
         LogOutput << "Database action(s) succeeded!" << newline;
         Success = true;
         break;
      case PGS_INITDB_OK:
         LogOutput << "Database initialized successfully." << newline;
         Success = true;
         break;
      case PGS_INITDB_ALREADY_EXISTS:
         LogOutput << "Database already exists, using it." << newline;
         Success = true;
         break;
      case PGS_BIN_NOT_FOUND:
         LogOutput << "Postgres executable not found in: " + BinDir << newline;
         break;
      case PGS_BIN_NOT_RUNNABLE:
         LogOutput << "Postgres is not runnable!" << newline;
         break;
      case PGS_DB_NOT_READY:
         LogOutput << "Postgres is not ready to accept connections!" << newline;
         break;
      case PGS_DB_NO_RESPONSE:
         LogOutput << "No response from Postgres! Is it running?" << newline;
         break;
      case PGS_ERR:
         LogOutput << "Sorry an unexpected error occured!" << newline;
         break;
      default:
         LogOutput << "Received an unknown PGSstatusCode: " << Status << newline;
         break;
   }
   return Success;
}
