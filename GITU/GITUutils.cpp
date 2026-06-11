// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUutils
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Wednesday 14 August 2024 - 09:43AM
// ---------------------------------------------------------------------------
#include <ctype.h>
#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <GITU/GITUutils.h>
#include <INI/INIdata.h>
COL_LOG_MODULE;

COLstring GITUquoteArgument(const COLstring& Arg){
   COL_FUNCTION(GITUquoteArgument);
   COLstring Out = Arg;
	if (Arg.find(" ") != npos && !Arg.isWhitespace()) {
		if (Arg[0] != '\"')              { Out = "\"" + Out; }
		if (Arg[Arg.size() - 1] != '\"') { Out += "\""; }
	}
	return Out;
}

void GITUreadGitFile(const COLstring& FileName, INIdata* pData) {
   COL_FUNCTION(GITUreadGitFile);
   if(!FILfileExists(FileName)) {
      COL_TRC("Could not find :" << FileName);
      return;
   }
   try {
      COLstring Content;
      FILreadFile(FileName, &Content);
      COL_VAR(Content);
      INIparse(Content, pData);
   } catch(const COLerror& E) {
      COL_TRC("Failed to read git file: " << E.description());
   }
}

void GITUsaveGitFile(const COLstring& FileName, const INIdata& Data){
	COL_FUNCTION(GITUsaveGitFile);
	COLstring FileContent;
	INIformat(Data, &FileContent);
	COL_VAR(FileContent);
	if(FileContent.isWhitespace()){
		COL_TRC("Removing empty file: " << FileName);
		FILdeleteFileIfExists(FileName);
	} else{
		// TODO - make sure we handle when .git folder isn't a git repo.
		FILcreateDirectoryTreeToFile(FileName, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
		FILwriteFile(FileName, FileContent);
	}
}

bool GITUvalidLibraryName(const COLstring& Name, COLstring* pError) {
   COL_FUNCTION(GITUvalidLibraryName);
   if(Name.isWhitespace()){
      *pError = "The library name cannot be empty.";
      return false;
   }
   // if(Name.size() > 10){
   //    *pError = "The library name '" + Name + "' is too long. Library names must not exceed 10 characters.";
   //    return false;
   // }
   for(int i = 0; i < Name.size(); ++i) {
      char c = Name[i];
      if(!isupper(c) && !COLisdigit(c)) {
         *pError = "The library name '" + Name + "' must contain only uppercase letters and numbers.";
         return false;
      }
   }
   return true;
}

// The component name is used to generate component GUID, which is used as the component
// directory name. Therefore we need to check for disallowed characters.
// Windows is more restrictive than POSIX, so it's used for all platforms.
// Reference: https://gist.github.com/doctaphred/d01d05291546186941e1b7ddc02034d3
bool GITUvalidComponentName(const COLstring& Name, COLstring* pError) {
   COL_FUNCTION(GITUvalidComponentName);
   return !FILnameIsInvalid(Name, pError);
}

COLstring GITUresolveKeyFromBranchUrl(const COLstring& Url, const COLstring& Branch) {
   COL_FUNCTION(GITUresolveKeyFromBranchUrl);
   return Branch.is_null() ? Url : Url + "#" + Branch;
}