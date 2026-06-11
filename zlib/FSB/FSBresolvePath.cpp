//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: FSBresolvePath
//
// Author: Matthew Nitsopoulos
//
// Description: This is used to resolve the command line commands used 
//              in the file browser
//
// Implementation
//---------------------------------------------------------------------------
#include <FSB/FSBresolvePath.h>

#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <FIL/FILstorageManager.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
COL_LOG_MODULE;

static void FSBfileBrowserResolveDirUp(const COLaddress& Address, const COLstring& PartialPath, const COLstring& UserPath) {
    COL_FUNCTION(FSBfileBrowserResolvePathImp);
    COLvar Response;
    COL_VAR2(PartialPath, UserPath);
    COLstring Filepath = PartialPath + UserPath;
    COL_VAR(Filepath);
#ifdef _WIN32
if ("/" == Filepath) Filepath = "C:\\";
COL_TRC("Converted / to C:\\");
#endif
    Filepath = FILexpand(Filepath);
    COL_TRC("After expand: " << Filepath);
    while(!Filepath.is_null() && !FILfileExists(Filepath)){  // find a valid parent
        COL_TRC(Filepath << " doest not exist");
        Filepath = FILparentDir(Filepath);
        COL_TRC("Checking its parent " << Filepath);
    }
    if(Filepath.is_null()){
        COL_TRC("Path is invalid");
        return COLrespondError(Address, "File path could not be resolved");
    }
    if (FILisDirectory(Filepath)) {
        Filepath = FILpathSepAdd(Filepath);
        COL_TRC(Filepath << " is directory");
        Response["is_file"] = false;
    }
    Filepath = FILpathDir(Filepath);
    COL_VAR(Filepath);
    Response["dir_path"] = Filepath;
    COLrespondSuccess(Address, Response);
}

static void FSBfileBrowserResolvePathImp(const COLaddress& Address, const COLstring& Path) {
    COL_FUNCTION(FSBfileBrowserResolvePathImp);
    COL_VAR(Path);
    COLvar Response;
    COLstring Filepath = Path;
    COLstring FullPath = Path;
    COL_VAR(Filepath);
#ifdef _WIN32
if ("/" == Filepath) Filepath = "C:\\";
COL_TRC("Converted / to C:\\");
#endif
    Filepath = FILexpand(Filepath);
    COL_TRC("After expand: " << Filepath);
    while(!Filepath.is_null() && !FILfileExists(Filepath)){  // find a valid parent
        COL_TRC(Filepath << " doest not exist");
        Filepath = FILparentDir(Filepath);
        COL_TRC("Checking its parent " << Filepath);
    }
    if(Filepath.is_null()){
        COL_TRC("Path is invalid");
        return COLrespondError(Address, "File path could not be resolved");
    }
    if (FILisDirectory(Filepath)) {
        Filepath = FILpathSepAdd(Filepath);
        COL_TRC(Filepath << " is directory");
        Response["is_file"] = false;
    } else {
        COL_TRC(Filepath << " is file");
        Response["is_file"] = true;
        Response["file_path"]  = Filepath;
        Filepath = FILpathDir(Filepath);
        COL_TRC("Resolve to its parent directory " << Filepath);
    }
    COL_VAR(Filepath);
    Response["dir_path"] = Filepath;
    COLrespondSuccess(Address, Response);
}

static void FSBfileBrowserResolveHelper(const COLaddress& Address, const COLstring& PartialPath, const COLstring& UserPath) {
    COL_FUNCTION(FSBfileBrowserResolveHelper);
    bool FileResult = true;
    if (UserPath.size() > 0 && UserPath[0] == *FIL_DIR_SEPARATOR)                  { COL_TRC("Full file path detected");     FSBfileBrowserResolvePathImp(Address, UserPath); }
    else if ( UserPath.size() > 0 && ( UserPath[0] == '.' && UserPath[1] == '.') ) { COL_TRC("Move up directory detected");  FSBfileBrowserResolveDirUp(Address, PartialPath, UserPath); }
    else                                                                           { COL_TRC("Relative file path detected"); FSBfileBrowserResolvePathImp(Address, PartialPath + UserPath); }
}

// "/file_browser/resolve_path"
void FSBfileBrowserResolvePath(const COLwebRequest& Request) {
   COL_FUNCTION(FSBfileBrowserResolvePath);
   COLvar Data = Request.Data;
   COL_VAR(Data);
   FSBfileBrowserResolveHelper( Request.Address, 
                                COLvarRequiredString(Data, "partial_path"),
                                COLvarRequiredString(Data, "user_path"));        
}
