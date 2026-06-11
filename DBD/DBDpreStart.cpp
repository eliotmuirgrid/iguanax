// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDpreStart
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 23 September 2022 - 06:45PM
// ---------------------------------------------------------------------------
#include <DBD/DBDinit.h>
#include <DBD/DBDmigration.h>
#include <DBD/DBDmodeStartup.h>
#include <DBD/DBDpreStart.h>
#include <DBD/DBDsetup.h>
#include <DBD/DBDworld.h>
#include <DIR/DIRpath.h>
#include <DRX/DRXutils.h>

#ifndef _WIN32
#  include <unistd.h>
#  include <pwd.h>
#  include <sys/types.h>
#endif

#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEButils.h>
#include <IWEB/IWEBload.h>

#include <LOGE/LOGEencrypt.h>

#include <SDM/SDMserver.h>
#include <COL/COLarray.h>

#include <CRH/CRHcrash.h>

#include <GUID/GUIDid.h>

#include <FIL/FILatomic.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILdirEnumerator.h>

#include <PRO/PROexecute.h>
#include <PRO/PROlaunch.h>

#include <INST/INSTstart.h>

#include <USR/USRgenerateAdminIfEmpty.h>

#include <CMD/CMDlineParser.h>
#include <SERV/SERVworkingDir.h>
#include <DIR/DIRexpandPath.h>
#include <DIR/DIRusernameLinux.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// if we detect iNTER.../IguanaX -> new spot
// - after we are done we can rename to iFW/IguanaX_old

static bool DBD_FRESH_INSTALL = false;

static bool DBDgetWorkingDirFromCommandLine(const CMDlineParser& Parser, COLstring* pOut) {
   COL_FUNCTION(DBDgetWorkingDirFromCommandLine);
   if (!Parser.isFlagPresent("working_dir")) { COL_TRC("Skip."); return false; }
   *pOut = Parser.flagArgument("working_dir");
   return true;
}

static bool DBDgetWorkingDirFromIniFile(COLstring* pOut) {
   COL_FUNCTION(DBDgetWorkingDirFromIniFile);
   if (SERVworkingDir().is_null()) { COL_TRC("Skip."); return false; }
   *pOut = SERVworkingDir();
   return true;
}

static bool DBDcreateAndSetWorkingDir(const COLstring& WorkingDir, COLstring* pError) {
   COL_FUNCTION(DBDcreateAndSetWorkingDir);
   COL_VAR(WorkingDir);
   try {
      COLstring CleanWorkingDir = FILpathResolveEnvironment(DIRexpandPath(WorkingDir));
      if (!FILfileExists(CleanWorkingDir)) {
         COL_TRC("Creating working dir " << CleanWorkingDir);
         DBD_FRESH_INSTALL = true;     // TODO: refactor and remove
         FILmakeFullDir(CleanWorkingDir, FIL_USER_RWX);
#ifdef __linux__
         COLstring Username;
         DBDgetLinuxSudoUser(&Username);
         DBDsetPermissions(Username, CleanWorkingDir);
#endif
      }
      FILsetWorkingDir(CleanWorkingDir);
      return true;
   } catch (const COLerror& Error) {
      *pError = Error.description();
      return false;
   }
}

bool DBDsetWorkingDir(const CMDlineParser& Parser, COLstring* pError) {
   COL_FUNCTION(DBDsetWorkingDir);
   COL_TRC("Command dir: " << FILworkingDir());
   COLstring WorkingDir;
   do {
      if (DBDgetWorkingDirFromCommandLine(Parser, &WorkingDir)) { break; }
      if (DBDgetWorkingDirFromIniFile(&WorkingDir))             { break; }
      WorkingDir = DIRconfigDirDefault();
   } while (false);
   bool Result = DBDcreateAndSetWorkingDir(WorkingDir, pError);
   COL_TRC("Working dir: " << FILworkingDir());
   return Result;
}

void DBDprepWorkingDir(const COLstring& LoadedWorkingDir, bool NoMigration){
   COL_FUNCTION(DBDprepWorkingDir);
   COLstring WorkingDir = FILworkingDir();
   COLcout << "Config Dir: " << WorkingDir;
   if (DBD_FRESH_INSTALL) { COLcout << " (created)"; }
   COLcout << newline;
   if(!NoMigration){
      COL_TRC("Created a new working dir - let's see if there is any old IguanaX data to migrate");
      FILmakeFullDir(FILpathAppend(FILworkingDir(), "resources"), FIL_USER_RWX); // create resources immediately for curl
      DBDmigration(WorkingDir);
      DBDmigration2(WorkingDir);
   }
   // This has to done here - not every time we load.
   COL_TRC("create default ssl cert and key");
   COLcout << "Creating self-signed SSL certificate and private key..." << newline;
   IWEBcreateDefaultCertKey();  // TODO this might bite us but the error code inside this routine prints out to standard out
   // Could probably move log dir setup to DBDinit or SDBIstartLogQueue
   COLstring LogDir, Error;
   if (!SDBsetUpLogDirectory(&LogDir, DRXhighAvailabilityEnabled(), &Error) && !Error.is_null()) {
      COLcout << "Failed to set up custom log directory: " << Error << newline;
   }
   COLcout << "Log Dir: " << FILpathResolve(SDBroot()) << newline;
}

void DBDgetLinuxSudoUser(COLstring* pUsername) {
   COL_FUNCTION(DBDgetLinuxSudoUser);
   const char* sudoUser = getenv("SUDO_USER");
   if(sudoUser != nullptr) {
      *pUsername = COLstring(sudoUser);
   } else {
      *pUsername = DIRusernameLinux();
   }
   COL_VAR(*pUsername);
}

// If we do --install then it does not migrate
// If we do --run it will migrate any old data that it can find

void DBDsetPermissions(const COLstring& Username, const COLstring& WorkingDir) {
   COL_FUNCTION(DBDsetPermissions);
   COL_VAR2(Username, WorkingDir);
   bool Success;
   COLstring Output, Err;
   int ExitCode;
   COLstring ParentFolder = DIRconfigRootDirDefault();
   COLstring Command;
   Command = "chown -R " + Username + ":" + Username + " " + ParentFolder;
   ExitCode = PROexecuteString("", Command, &Output, &Err);
   COL_VAR3(ExitCode, Output, Err);
   Command = "chown -R " + Username + ":" + Username + " " + WorkingDir;
   ExitCode = PROexecuteString("", Command, &Output, &Err);
   COL_VAR3(ExitCode, Output, Err);
}

void DBDinstall(DBDworld* pWorld, int argc, const char** argv){
   COL_FUNCTION(DBDinstall);
	pWorld->FreshInstall = DBD_FRESH_INSTALL;
   if (INSTrerunAsAdmin(argc, argv)){
      return;
   }
   pWorld->pServer->setOnRun(COLnewClosure0(&DBDinitInstall, pWorld, true));
   pWorld->pServer->runServer();
}

void DBDcheckForEncryption(DBDworld* pWorld) {
   COL_FUNCTION(DBDcheckForEncryption);
   COLstring Error;
   if (!SDBlogEncryptionEnabled()) {
      COL_TRC("Proceeding without log encryption");
      DBDinitNormal(pWorld);
   } else {
      COL_TRC("Log encryption is enabled. Need log encryption password.");
      DBDinitNormalWithEncryption(pWorld); // this puts us into "log unlock" mode - only api should be to receive a log unlock password.
   }
}

int DBDrun(DBDworld* pWorld){
   COL_FUNCTION(DBDrun);
	pWorld->FreshInstall = DBD_FRESH_INSTALL;
   COLcout << "Running service as command line application. (ctrl+C to exit)" << newline;
   // pWorld->pServer->setOnRun(COLnewClosure0(&DBDinitNormal, pWorld));
   pWorld->pServer->setOnRun(COLnewClosure0(&DBDcheckForEncryption, pWorld));
   pWorld->pServer->runServer();

   if (pWorld->ReturnFailure){
      COL_TRC("Returning exit failure so that we will trigger a restart.");
      throw COLerror("Throwing error to trigger restart.", 0);
   } else {
      return EXIT_SUCCESS;
   }
}