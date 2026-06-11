//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITlog
//
// Description: Implementation
//
// Author: Vismay Shah
// Date:   Fri July 09 2021
//---------------------------------------------------------------------------

#include <CFG/CFGconfigApi.h>
#include <CFG/CFGmap.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <GITU/GITUconfigFlag.h>
#include <GIT/GITcustomCommand.h>
#include <GIT/GITexecute.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
COL_LOG_MODULE;

// Used in the terminal.
COLstring GITcommandWithConfigFlag(const COLstring& Username, const COLstring& Command){
   COL_FUNCTION(GITvalidateCustomCommand);
   COLstring GitString    = Command.substr(0,3);
   COLstring ConfigString = GITUconfigFlag(Username);
   COLstring EndingString = Command.substr(3);
   return GitString + ConfigString + EndingString;
}

// we do not want to allow && or piping -- only single git commands
static bool GITvalidateCustomCommand(const COLstring& Command) {
   COL_FUNCTION(GITvalidateCustomCommand);
   return !(Command.substr(0, 4) != "git " || COLstringContains(Command, " && ") || COLstringContains(Command, " | "));
}



// we want to keep some errors to print in terminal -- this requires using COLrespondSuccess
static bool GITrunCommand(const COLstring& Username, const COLstring& ProjectPath, const COLstring& Command, COLstring* pOutput) {
   COL_FUNCTION(GITrunCommand);
   COLstring& Output = *pOutput;
   if(!GITvalidateCustomCommand(Command)) {
      Output = "Command either not allowed or contains restricted characters. Must be a single git command. No && or pipes.";
      return false;
   }
   COLstring FinalCommand = GITcommandWithConfigFlag(Username, Command);
   COL_VAR(FinalCommand);
   COLstring Out, Err;
   int ExitCode = GITexecuteString(ProjectPath, FinalCommand, &Out, &Err);
   Output =  ExitCode ? Err : Out + " " + Err;
   return true;
   // COL_VAR2(Out, Err);
   // return COLrespondSuccess(Address, ExitCode ? Err : Out + " " + Err);
}

void GITcustomCommandImpl(const COLstring& Username, const COLstring& Component, const COLvar& Commands, const COLaddress& Address){
   COL_FUNCTION(GITcustomCommandImpl);
   COLstring ProjectPath = CFGresolveRepoFile(Component, "", true);
   if(ProjectPath.is_null()) { 
      return COLrespondError(Address, ProjectPath); 
   }
   COLvar Result;
   Result.setArrayType();
   for (int i = 0; i < Commands.size(); i++) {
      const COLstring& Command = Commands[i];
      COLstring Output;
      bool Success = GITrunCommand(Username, ProjectPath, Command, &Output);
      Result[i] = Output;
   }
   return COLrespondSuccess(Address, Result);
}

// /git/custom_command
void GITcustomCommand(const COLwebRequest& Request) {
   COL_FUNCTION(GITcustomCommand);
   const COLvar& Data = Request.Data;
   COLstring ComponentId, Error;
   COLvar Commands       = COLvarRequiredObject(Request.Data, "commands");
   bool Development      = COLvarRequiredBool(Data, "development");
   if(!Development) { return COLrespondError(Request.Address, "Cannot modify file structure of a run repository."); }
   COL_VAR(Data.json(1));
   if (!CFGcomponentId(Data, &ComponentId, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, ComponentId, ROLid::EDIT_SCRIPT, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_SCRIPT));
   }
   GITcustomCommandImpl(Request.User, ComponentId, Commands, Request.Address);
}