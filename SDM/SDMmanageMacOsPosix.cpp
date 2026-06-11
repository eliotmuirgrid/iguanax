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


#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef __APPLE__

// Apple deprecated the Launchd c API in macOS 10.10 but it's better than running
// terminal commands using PROpopen. We will keep using this API until Apple
// removes it from macOS.  (John Qi ?)

// Disable "deprecated-declarations" errors
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include <launch.h>
#include <errno.h>

#include <PRO/PROexecute.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

bool SDMserverExists(const COLstring& Name, bool* pExists, COLstring* pError) {
   COL_FUNCTION(SDMserverExists);
   COLstring Command = "launchctl list " + Name;
   COLstring Err, Out;
   int ExitCode = PROexecuteString(".", Command, &Out, &Err);
   *pExists =  Out.find("Program") != npos;
   COL_VAR3(ExitCode, Out, Err);
   COL_VAR(*pExists);
   return true;
}

bool SDMserverRunning(const COLstring& Name, bool* pRunning, COLstring* pError) {
   COL_FUNCTION(SDMserverRunning);
   COLstring Command = "launchctl list " + Name;
   COLstring Err, Out;
   int ExitCode = PROexecuteString(".", Command, &Out, &Err);
   *pRunning = Out.find("PID") != npos;
   COL_VAR3(ExitCode, Out, Err);
   COL_VAR(*pRunning);
   return true;
}

static bool SDMlaunchControl(const COLstring& Name, const char* Action, COLstring* pError) {
   COL_FUNCTION(SDMlaunchControl);
   launch_data_t msg = launch_data_alloc(LAUNCH_DATA_DICTIONARY);
   launch_data_dict_insert(msg, launch_data_new_string(Name.c_str()), Action);
   launch_data_t resp = launch_msg(msg);
   launch_data_free(msg);
   if (resp == NULL) {
      COL_TRC("Error came back");
      *pError = strerror(errno);
      return false;
   }
   if (launch_data_get_type(resp) != LAUNCH_DATA_ERRNO) {
      launch_data_free(resp);
      *pError = "Failed to modify service state. Service returned unknown response.";
      COL_VAR(*pError);
      return false;
   }
   int error_code = launch_data_get_errno(resp);
   COL_VAR2(error_code, COLerrorString(error_code));
   if (error_code != 36) {
      launch_data_free(resp);
      *pError = COL_T("Failed to modify service state: ") + COLerrorString(error_code) + ":" + COLintToString(error_code);
      COL_VAR(*pError);
      return false;
   }
   COL_TRC("All ok?");
   launch_data_free(resp);
   return true;
}

inline COLstring SDMlaunchdConfigPath(const COLstring& ServiceName) {
   COL_FUNCTION(SDMlaunchdConfigPath);
   return FILpathResolveEnvironment("~/Library/LaunchAgents/" + ServiceName + ".plist");
}

bool SDMserverStart(const COLstring& Name, COLstring* pError){
   COL_FUNCTION(SDMserverStart);
   return SDMlaunchControl(Name, LAUNCH_KEY_STARTJOB, pError);
}

bool SDMserverStop(const COLstring& Name, COLstring* pError){
   COL_FUNCTION(SDMserverStop);
   return SDMlaunchControl(Name, LAUNCH_KEY_STOPJOB, pError);
}

bool SDMserverRemove(const COLstring& Name, COLstring* pError) {
   COL_FUNCTION(SDMserverRemove);
   bool Result = SDMlaunchControl(Name, LAUNCH_KEY_REMOVEJOB, pError);
   COLstring FilePath = SDMlaunchdConfigPath(Name);
   if (FILfileExists(FilePath)) {
      COL_TRC("Remove service file " << FilePath);
      FILremoveWithThrow(FilePath);
   }
   COL_VAR(Result);
   return Result;
}

void SDMlaunchdConfig2(const COLstring& Name, const COLstring& Executable, const COLstring& User, bool AutoStart, bool KeepAlive, COLstring* pConfig){
   COL_FUNCTION(SDMlaunchdConfig2);
   COLstring& Out = *pConfig;
   Out = R"(<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>Label</key>
	<string>)" + Name + R"(</string>
)";
if (!User.is_null()) {
    Out += R"(	<key>UserName</key>
	<string>)" + User + R"(</string>
)";
}
Out += R"(	<key>Program</key>
	<string>)" + Executable + R"(</string>
	<key>RunAtLoad</key>
)";

if (AutoStart) {
    Out += R"(	<true/>
)";
} else {
    Out += R"(	<false/>
)";
}

Out += R"(	<key>KeepAlive</key>
)"; // keep on its own line!
if (KeepAlive) {
    Out += R"(	<true/>
)";
} else {
    Out += R"(	<false/>
)";
}

Out += R"(</dict>
</plist>
)";
}

bool SDMserverInstall(const COLstring& Name, const COLstring& DisplayName, const COLstring& Description,
     const COLstring& Executable, const COLstring& User, const COLstring& Password, bool AutoStart, COLstring* pError){
   COL_FUNCTION(SDMserverInstall);
   COLstring Config;
   SDMlaunchdConfig2(Name, Executable, User, AutoStart, true, &Config);
   COL_VAR(Config);
   COLstring FilePath = SDMlaunchdConfigPath(Name);
   COL_VAR(FilePath);
   FILmakeFullDir(FILpathDir(FilePath), FIL_USER_RWX);
   FILwriteFile(FilePath, Config);
   FILsetPermission(FilePath, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   COLstring Out, Err;
   int ExitCode = PROexecuteString(".", "launchctl load " + FilePath, &Out, &Err);
   // TODO - error checking (duh)
   return true;
}

static int SDMmacDictFindIndex(const COLvar& Data, const COLstring& Key){
   COL_FUNCTION(SDMmacDictFindIndex);
   COL_VAR(Data.json(1));
   const COLvar& Root = Data["plist"]["dict"][0]["key"];
   COL_VAR(Root);
   int Index = 0;
   if(!Root.isArray()) { return -1; }
   for (auto i=Root.array().begin(); i!= Root.array().end(); i++){
      if ((*i)["text"].asString() == Key){
         return Index;
      }
      Index++;
   }
   return -1;
}

static COLstring SDMmacDictKeyValue(const COLvar& Data, const COLstring& Key){
   COL_FUNCTION(SDMmacDictKeyValue);
   int Index = SDMmacDictFindIndex(Data, Key);
   if (Index == -1){
      return "";
   }
   return Data["plist"]["dict"][0]["string"][Index]["text"].asString();
}

COLstring SDMserverGetProperty(const COLstring& Name, const COLstring& Property){
   COL_FUNCTION(SDMserverGetProperty);
   COLstring FilePath = SDMlaunchdConfigPath(Name);
   COLstring Content;
   FILreadFile(FilePath, &Content);
   Content = Content.substr(Content.find("<plist"));
   COLvar Xml;
   Xml.parseXml(Content);
   COLstring Value = SDMmacDictKeyValue(Xml, Property); // apple make this difficult
   COL_VAR4(Value, FilePath, Content, Xml.json(1));
   return Value;
}

bool SDMserverExe(const COLstring& Name, COLstring* pExe, COLstring* pError){
   COL_FUNCTION(SDMserverExe);
   try {
      *pExe = SDMserverGetProperty(Name, "Program");
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
      *pUser = SDMserverGetProperty(Name, "UserName");
      return true;
   } catch (const COLerror& Error) {
      *pError = Error.description();
      COL_ERR(*pError);
   }
   return false;
}

// Re-enable "deprecated-declarations" errors
#pragma GCC diagnostic pop
#endif