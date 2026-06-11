// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUuncommittedChanges
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Tuesday 21 March 2023 - 01:22PM
// ---------------------------------------------------------------------------

#include <CFG/CFGconfig.h>
#include <CFG/CFGmapApi.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GITU/GITUmodules.h>
#include <GIT/GITpath.h>
#include <GIT/GITstatusChanges.h>
#include <GITU/GITUuncommittedChanges.h>
#include <PRM/PRMcallbacks.h>
#include <ROL/ROLid.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void GITUchangesCallback() {
	COL_FUNCTION(GITUchangesCallback);
	COL_TRC("git/uncommitted_changes api complete");
}

static void GITUremoveQuotationMarksAroundName(COLstring& Name){
   COL_FUNCTION(GITUremoveQuotationMarksAroundName);
   COL_VAR(Name);
   if (Name.is_null()) return;
   // Edge case : Name = Samples/Sample_001 -> "Samples/msg 1"
   if ((Name[0] == '"' || Name[Name.size()-1] == '"') && Name.size() > 3) {
      COL_TRC("Name contains quotations around it that we need to remove");
      Name.replace("\"", ""); //Replaced substr with "replace" because when there are quotes in the center of a string, substr is unable to remove them.
   }
   COL_VAR(Name);
}

void GITUparseUncommittedOutput(const COLstring& RelativePath, const COLstring& GitStatusOutput, bool Uncommitted, const COLstring& ParentPath, COLvar* pOut){
   COL_FUNCTION(GITUparseUncommittedOutput);
   COLvar& Output = *pOut;
   COL_VAR(GitStatusOutput);
   COLvar Result;
   Result.setArrayType();
   COLstring Out1;
   COLstring Out2 = GitStatusOutput;
   COL_VAR(GitStatusOutput);
   int UpTo = 2;
   if(!Uncommitted){
      UpTo = 1;
   }
   while(Out2.split(Out1, Out2, "\n")) {
      COLstring Status = Out1.substr(0, UpTo);
      COLstring Name   = Out1.substr(UpTo+1);
      COLvar Out;
      GITUremoveQuotationMarksAroundName(Name);
      // Name comes from Git output, which always uses POSIX path separator. ParentPath could contain Window or POSIX path separator
      // Edge case : Name = comps/Custom_B3bwXVTSvjCsRz/dev/Samples/Sample_001 -> "comps/Custom_B3bwXVTSvjCsRz/dev/Samples/msg 1"
      if(COLstringHasPrefix(Name, FILpathSepToPosix(ParentPath))) Name.replace(FILpathSepToPosix(ParentPath), "");
      Out["name"]    = Name;
      Out["status" ] = Status;
      COLstring Path = FILpathAppend(RelativePath, Name);
      Out["path"]    = Path;
      Out["parent"]  = ParentPath;
      COL_VAR2(Name, Status);
      Result.push_back(Out);
   }
   if(Result.size() != 0) Output[RelativePath] = Result;
}

static void GITUuncommittedChangesCaller(const COLstring& ComponentId, const COLstring& ModuleName, bool Development, COLvar* pOut, COLstring* pError){
   COL_FUNCTION(GITUuncommittedChangesCaller);
   COLstring LibraryDir = GITUmoduleDirFull(ComponentId, ModuleName, Development);
   COLstring Dir = DIRcomponentProjectDir(ComponentId, Development);
   COLstring Content;
   if(!GITstatusChanges(LibraryDir, &Content, pError)) return;
   COLstring ConfigDir = DIRconfigDir();
   COLstring ParentPath = Dir;
   if(COLstringHasPrefix(Dir, ConfigDir)) ParentPath = COLstringCutPrefix(Dir, ConfigDir);
   GITUparseUncommittedOutput(ModuleName, Content, true, ParentPath, pOut);
}

static void GITUuncommittedChangesImpl(COLstring ComponentId, bool Development,  COLaddress Address, SCKloop* pLoop, COLclosure0* pCallback){
   COL_FUNCTION(GITUuncommittedChangesImpl);
	COLvar Result;
	const COLstring ComponentDir = DIRcomponentProjectDir(ComponentId, Development);
   COLmap<COLstring, GITUmodule> ModuleList;
   ModuleList.add("", GITUmodule("", ""));
   COLstring FileName = ComponentDir + ".gitmodules";
   if(FILfileExists(FileName)) GITUmoduleLoad(FileName, &ModuleList);
   COLstring Error;
   Result.setMapType();
   for (auto i=ModuleList.begin(); i != ModuleList.end(); ++i){
      COLstring ModuleName = i->first;
      GITUuncommittedChangesCaller(ComponentId, ModuleName, Development, &Result, &Error);
   }
	COLrespondSuccess(Address, Result);
	pLoop->post(pCallback);
}

// /git/uncommitted_changes
void GITUuncommittedChanges(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop) {
	COL_FUNCTION(GITUuncommittedChanges);
	COL_VAR(Request.Data.json(1));
	const COLstring Component = COLvarRequiredString(Request.Data, "component");
	bool Dev						  = COLvarRequiredBool(Request.Data, "development");
	ROLid Id						  = Dev ? ROLid::EDIT_SCRIPT : ROLid::VIEW_SCRIPT;
	CFGconfig* pConfig		  = CFGmapNode(pMap, Component);
	if(!pConfig) { return COLrespondError(Request.Address, "No component exists with name or id " + Component); }
	COLstring Error;
	if(!PRMcheckRoleAccess(Request.FromTranslator, Request.User, pConfig->CoreConfig.Guid, Id, &Error)) {
		return COLrespondError(Request.Address, PRMsetError(Id));
	}
	COLclosure0* pCallback = COLnewClosure0(GITUchangesCallback);
	pLoop->threadPool()->scheduleTask(COLnewClosure0(GITUuncommittedChangesImpl, pConfig->CoreConfig.Guid, Dev, Request.Address,
																	 pLoop, pCallback));
}
