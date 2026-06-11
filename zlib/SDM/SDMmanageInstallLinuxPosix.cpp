// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMmanage
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 12 July 2023 - 03:18PM
// ---------------------------------------------------------------------------

#include <SDM/SDMmanage.h>

#include <PRO/PROexecute.h>
#include <FIL/FILutils.h>

#include <INI/INIdata.h>

#include <USR/USRload.h>
#include <USR/USRpermissions.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef __linux__

static void SDMsystemdConfig(const COLstring& Name, const COLstring& Executable, const COLstring& User, COLstring* pConfig) {
   COL_FUNCTION(SDMsystemdConfig);
   COLstring& Out = *pConfig;
   Out = COL_T("[Unit]\n")
       + "Description=" + Name + "\n\n"
       + "[Service]\n";
   if (!User.is_null()) {
      Out += "User=" + User + "\n";
   }
   // TODO - this is hardcoded to autostart - should remove that option from the API if we alway autostart.
   Out += COL_T("Type=simple\n")
        + "ExecStart=" + Executable + "\n"
        + "Restart=always\n"
        + "RestartSec=5\n\n"
        + "[Install]\n"
        + "WantedBy=multi-user.target";
}

COLstring SDMsystemdConfigFilePath(const COLstring& Name) {
   COL_FUNCTION(SDMsystemdConfigFilePath);
   return "/etc/systemd/system/" + Name + ".service";
}

bool SDMserverRemove(const COLstring& Name, COLstring* pError) {
   COL_FUNCTION(SDMserverRemove);
   COLstring Out;
   int ExitCode = PROexecuteString(".", "systemctl disable " + Name + ".service", &Out, pError);
   COLstring FilePath = SDMsystemdConfigFilePath(Name);
   FILremove(FilePath);
   ExitCode = PROexecuteString(".", "systemctl daemon-reload", &Out, pError);
   return true;
}

bool SDMserverInstall(const COLstring& Name, const COLstring& DisplayName, const COLstring& Description,
     const COLstring& Executable, const COLstring& User, const COLstring& Password, bool AutoStart, COLstring* pError){
   COL_FUNCTION(SDMserverInstall);
   COL_VAR3(Executable, User, Password);
   if (!FILfileExists("/etc/systemd/system")) {
      *pError = "Could not detect /etc/systemd/system";
      return false;
   }
   COLstring Config;
   SDMsystemdConfig(Name, Executable, User, &Config);
   COLstring FilePath = SDMsystemdConfigFilePath(Name);
   COL_VAR(FilePath);
   FILwriteFile(FilePath, Config);
   FILsetPermission(FilePath, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   COLstring Out, Err;
   int ExitCode;
   ExitCode = PROexecuteString(".", "systemctl daemon-reload", &Out, &Err);
   if (ExitCode != 0){
      *pError = "Failed to reload daemon: " + Out + Err;
      return false;
   }
   ExitCode = PROexecuteString(".", "systemctl enable " + Name, &Out, &Err);
   if (ExitCode != 0){
      *pError = "Failed to enable daemon:" + Out + Err;
      return false;
   }
   //if (AutoStart){
   //   // Not sure if this will do the trick...TODO - Eliot
   //   return SDMserverStart(Name, pError);
   //}else {
   return true;
   //}
}

COLstring SDMserverProperty(const COLstring& Name, const COLstring& Section, const COLstring& Key){
   COL_FUNCTION(SDMserverProperty);
   COLstring FilePath = SDMsystemdConfigFilePath(Name);
   COL_VAR(FilePath);
   COLstring Content;
   FILreadFile(FilePath, &Content);
   INIdata Data;
   INIparse(Content,&Data);
   return INIdataKeyGet(Data, Section, "", Key);
}

bool SDMserverExe(const COLstring& Name, COLstring* pExe, COLstring* pError){
   COL_FUNCTION(SDMserverExe);
   try {
      *pExe = SDMserverProperty(Name, "Service", "ExecStart");
      return true;
   } catch (const COLerror& Error) {
      *pError = Error.description();
      COL_ERR(*pError);
   }
   return false;
}

bool SDMserverUser(const COLstring& Name, COLstring* pUser, COLstring* pError){
   COL_FUNCTION(SDMserverUser);
   try {
      *pUser = SDMserverProperty(Name, "Service", "User");
      return true;
   } catch (const COLerror& Error) {
      *pError = Error.description();
      COL_ERR(*pError);
   }
   return false;
}

#endif // __linux__
