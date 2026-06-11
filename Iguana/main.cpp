//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: main routine for IguanaX.
//
// Description:
//
// Main routine for IguanaX.
//
// Author: Henry Tran
// Date:   February 12, 2004
//---------------------------------------------------------------------------
#include <APP/APPmain.h>
#include <BLUA/BLUAsafe.h>
#include <CACHE/CACHEfile.h>
#include <CMD/CMDlineParser.h>
#include <CURL/CURLutils.h>
#include <DBD/DBDlaunchDashboard.h>
#include <DBD/DBDlistLocalIPs.h>
#include <DBD/DBDmodeStartup.h>
#include <DBD/DBDpreStart.h>
#include <DBD/DBDsetup.h>
#include <DBD/DBDstartupApple.h>
#include <DBD/DBDstartupLinux.h>
#include <DBD/DBDstartupWin.h>
#include <DBD/DBDtrace.h>
#include <DBD/DBDworld.h>
#include <DBDVER/DBDVERversionUtils.h>
#include <DIR/DIRpath.h>
#include <DIR/DIRusernameLinux.h>
#include <DRX/DRXiguanaId.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUdetect.h>
#include <IWEB/IWEBiguanaUrl.h>
#include <IWEB/IWEBportChange.h>
#include <LINK/LINKconstants.h>
#include <POP/POPcolor.h>
#include <SCK/SCKutils.h>
#include <SDB/SDBdir.h>
#include <SDM/SDMlogEvent.h>
#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>
#include <SDM/SDMserver.h>
#include <SERV/SERVaction.h>
#include <SERV/SERVconfig.h>
#include <SERV/SERVutils.h>
#include <SERV/SERVworkingDir.h>
#include <USR/USRsetAdmin.h>
#include <WEB/WEBcheck.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __linux__
#include <unistd.h>
#endif

#include <INI/INIdata.h>

#include <DBG/DBGdownloadPdb.h>

#include <GUID/GUIDid.h>

#include <INST/INSTstart.h>
#include <INST/INSTisStartup.h>
#include <COL/COLsimulateOs.h>

#include <CRH/CRHcrash.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDaddGlobalFlags(CMDlineParser* pParser){
   COL_FUNCTION(DBDaddGlobalFlags);
   #ifdef __APPLE__
   pParser->addFlagWithArgument("service", "<install|uninstall|status>", "Install, uninstall or check the status of the Iguana service");
   #else
   pParser->addFlagWithArgument("service", "<start|stop|restart|install|uninstall|status>", "Start, stop, restart, install, uninstall or check the status of the Iguana service");
   #endif
   pParser->addFlagWithoutArgument("open_gui", "Open Iguana dashboard in the default browser. Does not start iguana if it's not already started.");
   pParser->addFlagWithoutArgument("version", "Display Iguana version and exit.");
   pParser->addFlagWithoutArgument("id", "Display the Iguana Id.");
   pParser->addFlagWithoutArgument("info", "Display info about the iguana instance.");
   pParser->addFlagWithoutArgument("local_ips", "Display all IP addresses the localhost resolves to.");
   pParser->addFlagWithoutArgument("version_tag", "Display Iguana in form X_X_X.");
   pParser->addFlagWithoutArgument("reset_admin_user", "Create an admin user with an automatically generated password");
   pParser->addFlagWithArgument("set_admin_password", "Password", "Set the admin password");
   pParser->addFlagWithArgument("set_web_port", "port", "Set the web port to run iguana on");
   pParser->addFlagWithArgument("username", "user", "The username to run under when you are installing iguana as a service");
   pParser->addFlagWithArgument("password", "password", "The password to use when you are installing iguana as a service");
   pParser->addFlagWithoutArgument("run", "Run Iguana as a standalone process.");
   pParser->addFlagWithoutArgument("safe_mode", "Prevent automatic startup of channels which are normally set to start automatically. Also enabled automatically when the previous startup was incomplete.");
   pParser->addFlagWithArgument("working_dir", "<working directory>", "Changes the location of the working directory where the configuration is loaded from");
}

void DBDaddAppleFlags(CMDlineParser* pParser){
   COL_FUNCTION(DBDaddAppleFlags);
   pParser->addFlagWithoutArgument("controller", "Run as controller menu bar app.");
}

void DBDinfo(const COLstring& WorkingDir, const COLstring& ServiceFileConfig, COLostream& Out){
   COL_FUNCTION(DBDinfo);
   bool ServiceConfigPresent = !ServiceFileConfig.is_null();
   Out << newline;
   Out << "Instance Dir: " << WorkingDir      << newline
       << "Web UI:       " << IWEBiguanaUrl() << newline;
   if(ServiceConfigPresent){
   Out << "Service File: " << ServiceFileConfig << newline;
   }
   Out << newline;
}

void DBDiguanaId(COLostream& Out){
   COL_FUNCTION(DBDiguanaId);
   Out << DRXiguanaId() << newline;
}

void DBDcannotFindInstanceErrorOut(COLostream& Out){
   COL_FUNCTION(DBDcannotFindInstanceErrorOut);
   COLstring InstallCommand;
#ifdef __linux__
   InstallCommand = "'--service install'";
#else
   InstallCommand = "'--install' or '--service install'";
#endif
   Out << "Cannot complete action: unable to find an instance of iguana." << newline << newline <<  "To create one try using " << InstallCommand <<". "<< newline << newline;
}

bool DBDneedsExistingWorkingDir(CMDlineParser* pParser){
   COL_FUNCTION(DBDneedsExistingWorkingDir);
   return pParser->isFlagPresent("info") || pParser->isFlagPresent("set_admin_password") || pParser->isFlagPresent("set_web_port");
}

int DBDopengui(){
   COL_FUNCTION(DBDopengui);
   if (!DBDlaunchDashboard()){
      return EXIT_FAILURE;
   } else {
      IWEBportChangeInfo(COLcout);
      return EXIT_SUCCESS;
   }
}

static bool DBDinstallPrereq(const COLstring& WorkingDir, COLstring* pUsername){
   COL_FUNCTION(DBDinstallPrereq);
   if (!SERVisSudo()) { return false; }
   COLstring Path;
   if(!GITUinstalled(&Path)){
      GITUnotInstalled(COLcout);
      return false;
   }
   COL_TRC("Create working directory if it does not already exist");
   DBDprepWorkingDir(WorkingDir, true);
#ifdef __linux__
   if(pUsername->is_null()) {
      DBDgetLinuxSudoUser(pUsername);
   }
   DBDsetPermissions(*pUsername, WorkingDir);
#endif
   return true;
}

static bool DBDisSudo() {
#ifdef __linux__
   if (geteuid() == 0) {
      COLcout << "Please run this command as the IguanaX service user or the relevant config files may be inaccessible to the IguanaX service." << newline;
      return true;
   }
#endif
   return false;
}

int main(int argc, const char** argv) {
#ifdef COL_ENABLE_TRACING
   COL_LOG_INIT(argc, argv);
#endif
#ifndef IFW_RELEASE
   COLcout << "**************** Dev Build ****************" << newline;
#endif
   FILinitExecutableName(argv[0]); // for Posix daemons
   // enable tracing when running as a service - very useful - CHM_TRACE/CHM_TRACE_FILE
   DBDtrace();
   // Need to config the service here
   COLstring ServiceFileConfig;
   SERVloadServiceConfiguration(&ServiceFileConfig);
   SDMserver Server( SDMserviceName() );
   try {
      SCKinitWinsock();
      bool CacheLoad = CACHEload();  // Load the cache early so we can change behavior on it early.
      COL_VAR(CacheLoad);
      CMDlineParser LineParser;
      LineParser.setDescription("IguanaX");
      DBDaddGlobalFlags(&LineParser);
      #ifdef _WIN32
      LineParser.addFlagWithoutArgument("install", "Run Iguana in the install mode.");
      #endif // only windows
   #ifdef __APPLE__
      DBDaddAppleFlags(&LineParser);
   #endif
      COLsimulateOS();
      try{
        LineParser.parseArgsNoTrace(argc, argv);
      } catch(const COLerror& Error){
         COLcout << Error.description() << newline;
         return EXIT_FAILURE;
      }

      COL_VAR(CacheLoad);
      if (!WEBcheck()){
         COLcout << "Warning - problem with packaging /web/index.html is not present." << newline;
      }

      if (LineParser.parsingErrorsPresent(COLcout)) {
         LineParser.showUsage(COLcout);
         return EXIT_FAILURE;
      }
      // Utilities that do not need working dir
      if (LineParser.isFlagPresent("version")) {
         COLcout << DBDVERstartupVersionInfo();
         return EXIT_SUCCESS;
      }
      if (LineParser.isFlagPresent("local_ips")) {
         DBDlistLocalIPs();
         return EXIT_SUCCESS;
      }
      if (LineParser.isFlagPresent("version_tag")) {
         COLcout << DBDVERversionTag();
         return EXIT_SUCCESS;
      }
      BLUAsetSafeMode(LineParser.isFlagPresent("safe_mode"));
      COLstring Error;
      if (INSTexeIsStartup() && !INSTisAdmin()){ // IX-3906 Ensure we have admin shell before trying to create any directories to prevent creation of a working dir with the wrong permissions (on windows).
         INSTrerunAsAdmin(argc, argv);
         return EXIT_SUCCESS;
      }
      if (!DBDsetWorkingDir(LineParser, &Error)) { 
         COLcout << "Failed to set working dir: " << Error << newline;
         return EXIT_FAILURE;
      }
      COLstring WorkingDir = FILworkingDir();   // TODO: refactor and remove WorkingDir below
      COL_VAR(WorkingDir);
      // For determining Iguana Id, we need to know which working directory we are using
      if (LineParser.isFlagPresent("id")) {
         // COLcout << "Generating Iguana ID for directory: " << FILworkingDir() << newline; // Uncomment for debugging.
         DBDiguanaId(COLcout);
         return EXIT_SUCCESS;
      }
      DBDtrace();
      // Checks "info", "set_admin_password", "set_web_port"
      if(DBDneedsExistingWorkingDir(&LineParser) && WorkingDir.is_null()){
         COL_TRC("Command line utility requires that the working directory is initialized and it is not.");
         DBDcannotFindInstanceErrorOut(COLcout);
         return EXIT_FAILURE;
      }
      if (LineParser.isFlagPresent("info")) {
         DBDinfo(WorkingDir, ServiceFileConfig, COLcout);
         return EXIT_SUCCESS;
      }
      if (LineParser.isFlagPresent("set_admin_password")) {
         if (DBDisSudo()) { return EXIT_FAILURE; }
         USRsetAdmin(LineParser.flagArgument("set_admin_password"), COLcout);
         return EXIT_SUCCESS;
      }
      if (LineParser.isFlagPresent("reset_admin_user")) {
         if (DBDisSudo()) { return EXIT_FAILURE; }
         USRsetAdmin(GUIDnewId(16), COLcout);
         return EXIT_SUCCESS;
      }
      if (LineParser.isFlagPresent("set_web_port")){
         if (DBDisSudo()) { return EXIT_FAILURE; }
         IWEBportChange(atoi(LineParser.flagArgument("set_web_port").c_str()));
         return EXIT_SUCCESS;
      }

      if (LineParser.isFlagPresent("open_gui")) {
         // I put traceInit after this so the short cut to Iguana does not fail due to permission issues.
         return DBDopengui();
      }

      // service option handler must go before DBDsetConfigDir(), or iguana will create /root/.iNTERFACEWARE/IguanaX/ when installing service in Linux.
      if (LineParser.isFlagPresent("service")) {
         if (LineParser.isFlagPresent("working_dir")) {
            COLcout << "The --working_dir option can't be used with --service." << newline;
            COLcout << "To customize the working directory of the IguanaX service, please read:" << newline;
            COLcout << POPpop(LINKservConfig) << newline;
            return EXIT_FAILURE;
         }
         COLstring Username = LineParser.flagArgument("username");
         if(LineParser.flagArgument("service") == "install" && !DBDinstallPrereq(WorkingDir, &Username)){
            COL_TRC("Failed install preconditions and install preparation.");
            return EXIT_FAILURE;
         }
         // say warning there is no admin user you will need to use set_admin beforehand
         // the path to the systemd file thats installed
         // what url is being listened on
         // what needs to be done instead of setting the admin password user and so on
         return SERVaction(LineParser.flagArgument("service"), Username, LineParser.flagArgument("password"));
      }
      // bool Success = DBGextractPdbIfNeeded(&Error);
      // COL_VAR2(Success, Error);
      // COL_TRC("Remove old upgrade.");
      // SERVremoveOld();
      COL_TRC("Initialize curl");
      CACHEsaveCaBundle();
      CURLglobalInit();

      // TODO DBDworld doesn't need to be here anymore.
      DBDworld World;
      World.pServer = &Server;
#ifdef __APPLE__
      return DBDstartupApple(LineParser, WorkingDir, &World);
#endif
#ifdef _WIN32
      return DBDstartupWin(LineParser, WorkingDir, &World, argc, argv);
#endif
#ifdef __linux__
      return DBDstartupLinux(LineParser, WorkingDir, &World);
#endif
   }
   catch (const COLerror& Error) {
      COLcout << Error.description() << newline;
      SDMlogErrorEvent(SDMserviceName(), Error.description());
      // TODO - log to our own system log?
      INSTpauseIfNotWindowsService();
      return EXIT_FAILURE;
   }
   COL_INF("End of main");
   return EXIT_SUCCESS;
}
