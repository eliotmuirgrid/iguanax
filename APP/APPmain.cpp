// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: APPmain
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 06 July 2023 - 11:01AM
// ---------------------------------------------------------------------------
#include <csignal>
#include <APP/APPmain.h>

#include <IWEB/IWEBiguanaUrl.h>

#include <CACHE/CACHEfile.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <DIR/DIRpath.h>

#include <LINK/LINKconstants.h>

#include <CMD/CMDlineParser.h>

#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>
#include <SDM/SDMutils.h>

#include <INST/INSTinstall.h>

#include <GZIP/GZIPutils.h>

#include <PRO/PROexecute.h>
#include <PRO/PROweb.h>
#include <PRO/PROutils.h>
#include <PRO/PROkill.h>

#include <USR/USRload.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool APPactivateTracing(int argc, const char * argv[]){
   FILinitExecutableName(argv[0]);
   CMDlineParser Parser;
   Parser.parseArgs(argc, argv);
   if (Parser.parsingErrorsPresent(COLcout)){
      Parser.showUsage(COLcout);
      return false;
   }
   return true;
}

void APPexePath(char** ppData) {
   COL_FUNCTION(APPexePath);
   COLstring ExePath = FILexecutableDir() + FILexecutableFileName();
   COL_VAR(ExePath);
   *ppData = new char[ExePath.size() + 1];
   memcpy(*ppData, ExePath.c_str(), ExePath.size() + 1);
}

void APPservicePath(char** ppData) {
   COL_FUNCTION(APPservicePath);
   COLstring ServicePath, Error;
   if (!SDMserverExe(SDMserviceName(), &ServicePath, &Error)) {
      COL_TRC("Do not have permission to get service file path");
   }
   COL_VAR(ServicePath);
   *ppData = new char[ServicePath.size() + 1];
   memcpy(*ppData, ServicePath.c_str(), ServicePath.size() + 1);
}

void APPiguanaStart(){
   COL_FUNCTION(APPiguanaStart);
   try {
      COLstring Error;
      bool Result = INSTinstall(FILexecutableDir() + FILexecutableFileName(),"", "", &Error);
      if (!Result){
         COLcout << Error << newline;
      }
      COL_VAR2(Result, Error);
      //Result = SDMserverStart(SDMserviceName(), &Error);
      //if (!Result){
      //   COLcout << Error << newline;
      //}
      COL_VAR2(Result, Error);
   } catch (COLerror& Error){
      COLcout << Error.description() << newline;
   }
}

void APPiguanaHelp(){
   COL_FUNCTION(APPiguanaHelp);
   if (!PROweb(LINKhelpHome)){
      COLcout << "Failed to open the help website" << newline;
   }
}

void APPgitHelp(){
   COL_FUNCTION(APPgitHelp);
   if (!PROweb(LINKhelpGit)){
      COLcout << "Failed to open the git help website" << newline;
   }
}

void APPhomeSpacesHelp() {
   COL_FUNCTION(APPgitHelp);
   if(!PROweb(LINKhelpHomeSpace)) {
      COLcout << "Failed to open the home directory with spaces help website" << newline;
   }
}

void APPiguanaStop(){
   COL_FUNCTION(APPiguanaStop);
   try{
      APPiguanaUninstall();
      //COLstring Error;
      //bool Result = SDMserverStop(SDMserviceName(), &Error);
   } catch (COLerror& Error){
      COLcout << Error.description() << newline;
   }
}

void APPiguanaDashboard(){
   COL_FUNCTION(APPiguanaDashboard);
   PROweb(IWEBiguanaUrl());
}

bool APPiguanaStatus(){
   COL_FUNCTION(APPiguanaStatus);
   bool Running, Exists;
   COLstring Error;
   if (!SDMserverRunning(SDMserviceName(), &Running, &Error)) {
      COLcout << Error;
      return false;
   }
   if (!SDMserverExists(SDMserviceName(), &Exists, &Error)) {
      COLcout << Error;
      return false;
   }
   return Running;
}

void APPiguanaUninstall(){
   COL_FUNCTION(APPiguanaUninstall);
   bool Exists;
   COLstring Error;
   if (!SDMserverExists(SDMserviceName(), &Exists, &Error)) {
      COLcout << Error;
      return;
   }
   if (Exists) {
      SDMserverRemove(SDMserviceName(), &Error);
   }
}

// TODO very fragile code.
void APPloadResourceData(const char* pFilePath, char** ppData, int* pSize){
   COL_FUNCTION(APPloadResourceData);
   COL_VAR(pFilePath);
   COLstring Content, Compressed;
   if (CACHEexists() && CACHEreadFile(pFilePath, &Compressed)){
      COL_TRC("Loading from cache.");
      GZIPuncompress(Compressed, &Content);
   } else {
      COL_TRC("Loading from file");
      COLstring FilePath = FILexecutableDir() + ".." + pFilePath;
      FILreadFile(FilePath, &Content);
   }
   COL_DUMP("Content", Content.c_str(), Content.size());
   *ppData = new char[Content.size()];
   *pSize = Content.size();
   memcpy(*ppData, Content.c_str(), Content.size()); // double copying - oh well.  Not that critical in this context.

}

void APPfreeData(char* pData) {
   COL_FUNCTION(pData);
   delete pData;
}

bool APPlaunchAsController(){
   COL_FUNCTION(APPlaunchAsController);
#ifdef __APPLE__
   COL_VAR(FILexecutableDir());
   if (FILexecutableDir().find("/MacOS/") != npos){
      COL_TRC("We are running in the context of APP bundle");
      if (!SDMisUnderLaunchD()){
         COL_TRC("We are not being launched by launchd so I will start as a controller");
         return true;
      } else {
         COL_TRC("Must be running as Iguana itself");
      }
   }
#endif
   return false;
}

bool APPinstalled(){
   COL_FUNCTION(APPinstalled);
   COLstring AdminUserConfigFile = DIRconfigUserDir("admin") + "user.config";
   if (!FILfileExists(AdminUserConfigFile)){
      COL_TRC("Iguana hasn't been installed yet.");
      return false;
   }
   COL_TRC("Installed already");
   return true;
}

COLstring APPcontrollerPidFile(){
   COL_FUNCTION(APPcontrollerPidFile);
	COLstring FileName = FILpathAppend(DIRresourcesDir(), "mac_controller.pid");
   COL_VAR(FileName);
   return FileName;
}

void APPwriteControllerPid(){
   COL_FUNCTION(APPwriteControllerPid);
   COLstring FileName = APPcontrollerPidFile();
   COLstring Content = COLintToString(PROpid());
   COL_VAR2(FileName, Content);
   FILwriteFile(FileName, Content);
}

// Assumes that control pid file exists.
int APPcontrollerPid(){
   COL_FUNCTION(APPcontrollerPid);
   COLstring Content;
   FILreadFile(APPcontrollerPidFile(), &Content);
   COL_VAR(Content);
   int Pid = atoi(Content.c_str());
   COL_VAR(Pid);
   return Pid;
}

bool APPkillMacController(){
   COL_FUNCTION(APPkillMacController);
   COLstring FileName = APPcontrollerPidFile();
   COL_VAR(FileName);
   COLstring Content;
   if (!FILfileExists(FileName)){
      COL_TRC("No controller PID file found.");
      return false;
   }
   FILreadFile(FileName, &Content);
   COL_VAR(Content);
   int Pid = atoi(Content.c_str());
   COL_TRC("About to kill PID " << Pid);
   PROkill(Pid);
   return true;
}

bool APPisControllerRunning() {
   COL_FUNCTION(APPisControllerRunning);
   if(!FILfileExists(APPcontrollerPidFile())) {
      COL_TRC("No controller PID file found - so not running");
      return false;
   }
#ifndef _WIN32
   int ControllerPid = APPcontrollerPid();
   if(kill(ControllerPid, 0) == 0) {
      COLcout << "Process with PID " << ControllerPid << " is running." << newline;
      return true;
   } else {
      COLcout << "Process with PID " << ControllerPid << " is not running." << newline;
      return false;
   }
#else
   // Does not run on windows
   return false;
#endif
}

bool APPiguanaLogExists() { return FILfileExists(FILworkingDir() + "ServiceErrorLog.txt"); }

bool APPiguanaServiceLog() {
   COL_FUNCTION(APPiguanaServiceLog);
   COLstring File = FILpathAppend(FILworkingDir(), "ServiceErrorLog.txt");
   if(!FILfileExists(File)) { return false; }
   COLstring Out, Err;
   int Exit = PROexecuteString("", "open " + File, &Out, &Err);
   return true;
}

bool APPshowStartupPrompt() {
   COL_FUNCTION(APPshowStartupPrompt);
   COLmap<COLstring, USRuser> Users;
   USRload(&Users);
   if(Users.count("admin") == 0) {
      COL_TRC("No users setup -- showing dialogue");
      return true;
   }
   if(USRmapAuthenticate(&Users, "admin", "password")) {
      COL_TRC("Default admin account exists -- showing dialogue");
      return true;
   }
   return false;
}

bool APPhomeHasSpaces() {
   COL_FUNCTION(APPhomeHasSpaces);
#ifdef _WIN32
   return false;
#endif
   COLstring Home = FILexpand("~");
   COL_VAR(Home);
   return Home.find(' ') != npos;
}