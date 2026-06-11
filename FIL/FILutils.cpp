//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILutils
//
// Description:
//
// Basic wrapping of operating system file functions
//
// Author: Eliot Muir
// Date:   Tue 09/01/1998
//---------------------------------------------------------------------------
#pragma hdrstop

#include "FILfile.h"
#include "FILdirEnumerator.h"
#include "FILosDefs.h"
#include "FILpathUtils.h"
#include "FILutils.h"

#include <COL/COLmap.h>
#include <COL/COLutils.h>
#include <COL/COLioCheck.h>
#include <COL/COLerror.h>
#include <COL/COLsplit.h>
#include <COL/COLstack.h>
#include <COL/COLthread.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#ifdef _WIN32
#   include <COL/COLwindows.h>
#   include <io.h>
#   include <direct.h>
#   include <winbase.h>
    const COLuint32 FIL_READABLE = 4;
    const COLuint32 FIL_WRITABLE = 2;
#   include <Shobjidl.h>
#   include <ShellAPI.h>
#   include <comdef.h>
#   include <objbase.h>

#else // POSIX
#   include <pwd.h>
#   include <unistd.h>
#   include <ftw.h>

#   ifdef __APPLE__
#      include <mach-o/dyld.h>
#   endif
    const COLuint32 FIL_READABLE = R_OK;
    const COLuint32 FIL_WRITABLE = W_OK;
    const COLuint32 FIL_EXECUTABLE = X_OK;
#endif // _WIN32

static COLstring s_ExecutableFileDirectory;
static COLstring s_ExecutableFileName;

const COLuint32 FIL_MAGIC_NUMBER = 1415926535;

static void FILsimulateError(const COLstring& FileName){
   COL_FUNCTION(FILsimulateError);
   if (!getenv("CHM_FILE_ERR")){
      return;
   }
   if (COLglobMatch(getenv("CHM_FILE_ERR"), FileName.c_str())){
      COL_ERR("Simulated file error on " << FileName << " raised.");
      throw COLerror("Cannot load " + FileName + " due to simulated file error from CHM_FILE_ERR=" +getenv("CHM_FILE_ERR"), 0);
   }
}

// Warning! The destination COLstring can have, at most, a capacity of
// MAX_INT, while the files we could be reading into it could have a size
// of FILindex, which is a COLuint64 (long long or __int64). Coercing this
// to an int might have some surprising results.
void FILreadFile(const COLstring& FileName, COLstring* pDestination){
   COL_FUNCTION(FILreadFile);
   FILsimulateError(FileName);
   FILfile File(FileName, FILfile::Read);
   int FileSize = File.size();
   COL_VAR(FileSize);
   pDestination->setCapacity(FileSize);
   COL_VAR(pDestination->capacity());
   pDestination->setSize(FileSize);
   File.read((void*)pDestination->c_str(), FileSize);
   int FileSize2 = File.size();
   File.close();
   if (FileSize2 != FileSize){
      throw COLerror("File " + FileName + " changed in size from " + COLintToString(FileSize) + " to " +  COLintToString(FileSize2) + " while reading it.");
   }
}

void FILwriteFile(const COLstring& FileName, const COLstring& Data){
   COL_FUNCTION(FILwriteFile);
   FILsimulateError(FileName);
   COLassertIfMainThread();
   FILfile File(FileName, FILfile::Rewrite);
   File.write((void*)Data.c_str(), Data.size());
   File.close();
}

void FILappendFile(const COLstring& FileName, const COLstring& Data){
   COL_FUNCTION(FILappendFile);
   COLassertIfMainThread();
   FILfile File(FileName, FILfile::Append);
   File.write((void*)Data.c_str(), Data.size());
   File.close();
}

void FILaddPathSeparator(COLstring& Directory){
   if (Directory.size() == 0){
      Directory += FIL_DIR_SEPARATOR;
      return;
   }
   if (Directory[Directory.size()-1] != FIL_DIR_SEPARATOR[0]){
      Directory += FIL_DIR_SEPARATOR;
   }
}

void FILremovePathSeparator(COLstring& Path){
   if (Path.is_null()){
      return;
   }
   if ( (Path.size() != 1) && (Path[Path.size()-1] == FIL_DIR_SEPARATOR[0]) ){
      Path = Path.substr(0, Path.size()-1);
   }
}

void FILpathSepLeadingRemoveOneNative(COLstring& Path){
   if (Path.is_null()){
      return;
   }
   if ( Path[0] == FIL_DIR_SEPARATOR[0] ){
      Path = Path.substr(1, Path.size());
   }
}

time_t FILmodified(const COLstring& FileName){
   COL_FUNCTION(FILmodified);
   COLassertIfMainThread();
   struct FILstat StatBuffer;
   int Result = FILstat(FileName.c_str(), &StatBuffer);
   if (Result != 0){
      COL_THROW_STRERROR_PLAIN("FILmodified on " << FileName << " failed.");
   }
   return StatBuffer.st_mtime;
}

void FILcorrectPathSeparators(COLstring& Path) {
#ifdef _WIN32
   const char FromChar = '/';
   const char ToChar = '\\';
#else
   const char FromChar = '\\';
   const char ToChar = '/';
#endif
   char* Pos = (char*)Path.c_str();
   while (char Ch = *Pos){
      if (FromChar == Ch){
         *Pos = ToChar;
      }
      ++Pos;
   }
}

#ifdef _WIN32
bool FILfileExists(const COLstring& FileName) {
   COL_FUNCTION(FILfileExists);
   COLassertIfMainThread();
   COL_VAR(FileName);
   COLstring WindowsName;
   int Result = _waccess(FILmangleFileNameForWindows(FileName, &WindowsName), FILexistsPermissions);
   COL_VAR(Result);
   return Result == 0;
}
#else
bool FILfileExists(const COLstring& FileName) {
   COL_FUNCTION(FILfileExists);
   COLassertIfMainThread();
   int Result = FILaccess(FileName.c_str(), FILexistsPermissions);
   return Result == 0;
}
#endif

bool FILfileExists(const COLstring& FileName, FILnameFilter pFilter) {
   COL_FUNCTION(FILfileExists);
   if (pFilter.get()) {
      COLstring Tmp;
      pFilter->run(FileName, &Tmp);
      return FILfileExists(Tmp);
   }
   return FILfileExists(FileName);
}

// Pretty brute force but it is accurate.
bool FILfileReadWritable(const COLstring& FileName){
   COL_FUNCTION(FILfileReadWritable);
   FILE* F = fopen(FileName.c_str(), "r+");
   if (F != NULL) {
     fclose(F);
     return true;
   } else {
     return false;
   }
}

bool FILexecutableExists(const COLstring& CommandLine) {
   COL_FUNCTION(FILexecutableExists);
   COLarray<COLstring> Pieces;
   COLsplit(&Pieces, CommandLine, " ");
   COLstring Trying;
   for (COLarray<COLstring>::const_iterator i = Pieces.begin(); i != Pieces.end(); ++i) {
      COL_TRC(i->data());
      Trying.append(i->data());
      if (FILfileExists(Trying)) {
         return true;
      }
      Trying.append(" ");
   }
   return false;
}

bool FILhasPermission(const COLstring& FileName, COLuint32 PermissionMode){
   if(!FILfileExists(FileName)){
      COL_ERROR_STREAM_PLAIN("File `" << FileName << "\' does not exist.", COLerror::FileError);
   }
   return (FILaccess(FileName.c_str(), PermissionMode) == 0);
}

#ifndef _WIN32
// PerimissionMode example    TODO : is it possible to change it to match chmod convention? e.g. 744 for rwxr--r--
// ls -l   : rwxr--r--
// Binary  : 111100100
// Decimal : 484
void FILsetPermission(const COLstring& FileName, COLuint32 PermissionMode){
   if (chmod(FileName.c_str(), PermissionMode) != 0){
      COL_THROW_STRERROR_PLAIN("Attempt to change permissions on '" << FileName << "' failed.");
   }
}
#else
void FILsetPermission(const COLstring& FileName, COLuint32 PermissionMode){
   COL_ERR("FILsetPermission does nothing under windows for this file: " << FileName);
}
#endif

// TODO - make a version of this doesn't throw
COLstring FILworkingDir(){
   char Buffer[_MAX_PATH];
   if (NULL == getcwd(Buffer, _MAX_PATH)){
      int LastError = COLlastError();
      COLstring Error = COLerrorString(LastError);
      throw COLerror(Error, COLlastError());
   }
   COLstring Directory = Buffer;
   FILaddPathSeparator(Directory);
   return Directory;
}

bool FILworkingDirExists(){
   COL_FUNCTION(FILworkingDirExists);
   char Buffer[_MAX_PATH];
   if (NULL == getcwd(Buffer, _MAX_PATH)){
      int LastError = COLlastError();
      COLstring Error = COLerrorString(LastError);
      COL_VAR2(LastError, Error);
      return false;
   }
   return true;
}

#ifdef _WIN32
void FILsetWorkingDir(const COLstring& Dir) {
   COL_FUNCTION(FILsetWorkingDir);
   COL_INF("FILsetWorkingDir: " << Dir);
   COLstring WinDir;
   if (0 != _wchdir(FILmangleFileNameForWindows(Dir, &WinDir))) {
      COL_ERR("FILsetWorkingDir failed: " << Dir);
      COL_THROW_STRERROR_PLAIN("Attempt to change the working directory to '" << Dir << "' failed.");
   }
}

void FILmakeDir(const COLstring& Dir, const COLint32 Mode, const char* pOwnerUserName) {
   COL_FUNCTION(FILmakeDir);
   COLstring WindowsDir;
   if (0 != _wmkdir(FILmangleFileNameForWindows(Dir, &WindowsDir))) {
      COL_THROW_STRERROR_PLAIN("Attempt to make new directory '" << Dir << "' failed.");
   }
}
#else
void FILsetWorkingDir(const COLstring& Dir) {
   COL_FUNCTION(FILsetWorkingDir);
   COL_INF("FILsetWorkingDir: " << Dir);
   if (0 != FILchdir(Dir.c_str())) {
      COL_ERR("FILsetWorkingDir failed: " << Dir);
      COL_THROW_STRERROR_PLAIN("Attempt to change the working directory to '" << Dir << "' failed.");
   }
}

void FILmakeDir(const COLstring& Dir, const COLint32 Mode, const char* pOwnerUserName) {
   COL_FUNCTION(FILmakeDir);
   COL_VAR3(Dir, Mode, pOwnerUserName);
   if (0 != mkdir(Dir.c_str(), Mode)) { // Mode definitions are in FILutils.h
      COL_THROW_STRERROR_PLAIN("Attempt to make new directory '" << Dir << "' failed.");
   }

   if (pOwnerUserName) {
      passwd* pUserInfo = ::getpwnam(pOwnerUserName);
      if (!pUserInfo) {
         COL_ERROR_STREAM_PLAIN("Invalid owner username `" << pOwnerUserName
            << "', or other error from ::getpwnam().", COLerror::PreCondition);
      }

      if (::chown(Dir.c_str(), pUserInfo->pw_uid, pUserInfo->pw_gid) == -1) {
         COL_ERROR_STREAM_PLAIN("The chown failed trying to change ownership of path `" << Dir << "' to user `" << pOwnerUserName << "'.", COLerror::SystemError);
      }
   }

}
#endif

void FILcreateDirectoryTreeToFile(const COLstring& FilePath, const COLint32 Mode){
   COL_FUNCTION(FILcreateDirectoryTreeToFile);
   int DirTreePathLength;
   FilePath.find_last_of(&DirTreePathLength, FIL_PATH_SEP_NATIVE);
   COLstring DirTree = FilePath.substr(0, DirTreePathLength + 1);
   COL_VAR(DirTree);
   FILmakeFullDir(DirTree, Mode);
}

void FILmakeFullDir(const COLstring& DirPath, const COLint32 Mode /* =0 */, const char* pOwnerUserName){
   COL_FUNCTION(FILmakeFullDir);
   COLassertIfMainThread();
   COL_VAR2(DirPath, Mode);
   COLstring TempDir(DirPath);
   FILaddPathSeparator(TempDir);
   COL_VAR2(TempDir, FILworkingDir());
   COLuint32 DirIndex = 0;
   TempDir = FILpathDirResolved(TempDir, FILworkingDir());
   COL_VAR(TempDir);
   COLarray<COLstring> Dirs;
   COLsplit(&Dirs, TempDir, FIL_PATH_SEP_NATIVE);
   COL_VAR(Dirs.size());
   if(Dirs.size() <= 1) { throw COLerror("This dir path is not valid for this platform: " + DirPath); }
   Dirs.remove(0);
   Dirs.remove(Dirs.size()-1);
   COL_VAR(Dirs.size());
   //for (int i=0; i< Dirs.size(); i++){   COL_VAR(Dirs[i]);}
   COL_VAR(FILpathVolumeResolved(TempDir));
   COLstring BaseDir = FILpathVolumeResolved(TempDir, FILworkingDir());
   COL_VAR(BaseDir);
   FILaddPathSeparator(BaseDir);
   COL_VAR(BaseDir);
   while (DirIndex < Dirs.size()) {
      BaseDir += Dirs[DirIndex];
      COL_VAR(BaseDir);
      if (FILfileExists(BaseDir)) {
         COL_TRC(BaseDir << " exists!");
         if (!FILisDirectory(BaseDir.c_str())) {
            COL_ERROR_STREAM_PLAIN("Error creating directory for '" << DirPath << "', '" << BaseDir << " is a file, not a directory.", COLerror::PreCondition);
         } else {
            FILaddPathSeparator(BaseDir);
         }
      } else {
         FILaddPathSeparator(BaseDir);
         try {
            COL_TRC("making " << BaseDir);
            FILmakeDir(BaseDir, Mode, pOwnerUserName);
         } catch (...) {
            if (!FILfileExists(BaseDir) || !FILisDirectory(BaseDir.c_str()) ) {
               throw;
            }
         }
      }
      DirIndex++;
   }
}

COLuint64 FILfileSize(const COLstring& FileName){
   COL_FUNCTION(FILfileSize);
   COLassertIfMainThread();
   struct FILstat StatBuffer;
   if (0 != FILstat(FileName.c_str(),&StatBuffer)){
     COL_THROW_STRERROR_PLAIN("Could not get file size for file '" << FileName << "'.");
   }
   return StatBuffer.st_size;
}

#ifdef _WIN32

static void normalizeWindowsPath(COLstring& Path){
   COL_FUNCTION(normalizePath);
   COL_TRC("Normalizing path " << Path);
   Path.replace('/', '\\');
   COL_TRC("Path normalized to " << Path);
}
static int removeFullDirNative(const COLstring& DirectoryName) {
   COL_FUNCTION(removeFullDirNative);
   COLstring DirectoryNameNormalized = FILpathResolve(DirectoryName, FILworkingDir());
   normalizeWindowsPath(DirectoryNameNormalized);
   COL_VAR(DirectoryNameNormalized);

   HRESULT OperationResult = S_OK;
   COLstack<COLstring> DirStack; // Stack to manage directories
   COLarray<COLstring> DirsToRemove; // Vector to track directories to remove after all contents are processed

   // Start with the initial directory
   DirStack.push(DirectoryNameNormalized);

   // Process all directories
   while (!DirStack.empty() && SUCCEEDED(OperationResult)) {
       // Get the next directory to process
       COLstring CurrentDir = DirStack.top();
       DirStack.pop();

       // Remember this directory for deletion after we've processed all its contents
       DirsToRemove.push_back(CurrentDir);

       // Prepare search path for FindFirstFile
       COLstring SearchPathStr = CurrentDir;
       if (SearchPathStr[SearchPathStr.size() - 1] != '\\') {
           SearchPathStr += '\\';
       }
       SearchPathStr += '*';

       // Convert to wide string for Windows API
       COLstring SearchWinDirName;
       wchar_t* pSearchWinDir = FILmangleFileNameForWindows(SearchPathStr, &SearchWinDirName);

       // Find all files and subdirectories
       WIN32_FIND_DATAW findData;
       HANDLE hFind = FindFirstFileW(pSearchWinDir, &findData);

       if (hFind == INVALID_HANDLE_VALUE) {
           OperationResult = HRESULT_FROM_WIN32(GetLastError());
           COL_TRC("Error!");
           _com_error ErrorObj(OperationResult);
           COLstring ErrorMessage(ErrorObj.ErrorMessage());
           COL_TRC("Error trying to list contents of " + CurrentDir + ". " + ErrorMessage + ".");
           continue; // Try to process other directories in the stack
       }

       // Process all items in the current directory
       do {
           // Skip "." and ".." directories
           if (wcscmp(findData.cFileName, L".") == 0 || wcscmp(findData.cFileName, L"..") == 0) {
               continue;
           }

           // Create full path for the current item
           COLstring FileName;
           char* tempFileName = new char[wcslen(findData.cFileName) + 1];
           wcstombs(tempFileName, findData.cFileName, wcslen(findData.cFileName) + 1);
           FileName = tempFileName;
           delete[] tempFileName;

           COLstring FullPath = CurrentDir;
           if (FullPath[FullPath.size() - 1] != '\\') {
               FullPath += '\\';
           }
           FullPath += FileName;

           if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
               DirStack.push(FullPath);  // If it's a directory, push it onto the stack for processing
           } else {
               COLstring FileWinDirName; // If it's a file, delete it
               wchar_t* pFileWinDir = FILmangleFileNameForWindows(FullPath, &FileWinDirName);

               SetFileAttributesW(pFileWinDir, FILE_ATTRIBUTE_NORMAL);
               if (!DeleteFileW(pFileWinDir)) {
                   OperationResult = HRESULT_FROM_WIN32(GetLastError());
                   COL_TRC("Error!");
                   _com_error ErrorObj(OperationResult);
                   COLstring ErrorMessage(ErrorObj.ErrorMessage());
                   COL_TRC("Error trying to remove file " + FullPath + ". " + ErrorMessage + ".");
               }
           }
       } while (SUCCEEDED(OperationResult) && FindNextFileW(hFind, &findData));

       // Close the find handle
       FindClose(hFind);
   }

   // Now remove all directories in reverse order (children before parents)
   for(int i = DirsToRemove.size() - 1; i >= 0 && SUCCEEDED(OperationResult); --i) {
      const COLstring& CurrentDir = DirsToRemove[i];
      // Convert to wide string for Windows API
      COLstring WinDirName;
      wchar_t* pWinDir = FILmangleFileNameForWindows(CurrentDir, &WinDirName);
      // Make sure directory is not read-only
      SetFileAttributesW(pWinDir, FILE_ATTRIBUTE_NORMAL);
      // Delete the directory
      if(!RemoveDirectoryW(pWinDir)) {
         OperationResult = HRESULT_FROM_WIN32(GetLastError());
         COL_TRC("Error!");
         _com_error ErrorObj(OperationResult);
         COLstring ErrorMessage(ErrorObj.ErrorMessage());
         COL_TRC("Error trying to remove directory " + CurrentDir + ". " + ErrorMessage + ".");
      }
   }
   return OperationResult;
}

#else

static int removeFullDirPosixDirTreeWalkCallback(const char* FilePath, const struct stat* Stats, int TypeFlag, struct FTW* OptionFlags){
   COL_FUNCTION(removeFullDirPosixDirTreeWalkCallback);
   COL_TRC("About to remove file/directory: " << FilePath);
   int RemoveResult = remove(FilePath);
   if(RemoveResult == -1){
      COL_TRC("Error trying to remove " + COLstring(FilePath) + ". " + COLstring(strerror(errno)) + ".");
   }
   return RemoveResult;
}
static int removeFullDirNative(const COLstring& DirectoryName){
   COL_FUNCTION(removeFullDirNative);
   COL_TRC("About to recursively remove directory: " + DirectoryName);
   return nftw(DirectoryName.c_str(), removeFullDirPosixDirTreeWalkCallback, 50, FTW_DEPTH | FTW_PHYS);
}

#endif

static void FILremoveFullDirNewImpl(const COLstring& DirectoryName){
   COL_FUNCTION(FILremoveFullDirNewImpl);
   int RemoveResult;
   COL_TRC("Calling relevant OS implementation code..");
   RemoveResult = removeFullDirNative(DirectoryName);
   if(RemoveResult < 0){
      throw COLerror("Attempt to remove '" + DirectoryName + "' failed. Error " + COLint32ToString(RemoveResult));
   }
   COL_TRC("Successfully removed entire directory: " + DirectoryName);
}

bool FILremoveFullDirNewSafe(const COLstring& DirectoryName, COLstring* pError){
   COL_FUNCTION(FILremoveFullDirNewSafe);
   if (!FILfileExists(DirectoryName)) { return true; } // don't try to delete if it doesn't exist.
   try { 
      FILremoveFullDirNew(DirectoryName);
      return true;
   } catch (const COLerror& Error){
      *pError = Error.description();
      return false;
   }
}

void FILremoveFullDirNew(const COLstring& DirectoryName){
   COL_FUNCTION(FILremoveFullDirNew);
   // We allow 2 retries to overcome failures due to hidden file auto-regeneration,
   // or other unexpected file system errors.
   int MaxRetries = 2;
   int RetryCount = 0;
   while(true){
      try{
         FILremoveFullDirNewImpl(DirectoryName);
         break;
      }
      catch(COLerror& Error){
         if(RetryCount == MaxRetries){
            COL_TRC("Remove failed and we reached our retry limit.");
            throw Error;
         }
         COL_TRC("Remove failed and still under retry limit. Will try to remove dir again...");
         COL_VAR2(RetryCount, MaxRetries);
         RetryCount++;
      }
   }
}

bool FILisDirectoryEmpty(const COLstring& DirectoryName){
   COL_FUNCTION(FILisDirectoryEmpty);
   bool Result = true;

   COLstring Dir(DirectoryName);
   FILaddPathSeparator(Dir);

#ifdef _WIN32
   FILdirEnumerator DirEnumerator(Dir + "*.*");
#else // POSIX
   FILdirEnumerator DirEnumerator(Dir + "*");
#endif

   COLstring Dummy;
   while( DirEnumerator.getFile(Dummy) ){
      COL_VAR(Dummy);
      Result = false;
   }
   return Result;
}

#ifdef _WIN32
bool FILisDirectory(const COLstring& FullPath) {
   COL_FUNCTION(FILisDirectory);
   COLstring Path = FullPath;
   FILcorrectPathSeparators(Path);
   FILremovePathSeparator(Path);
   if(Path.size() == 2 && isalpha(Path[0]) && Path[1]==':') {
      // a drive-root path, e.g. C:\
      FILaddPathSeparator(Path);
   }
   COLstring WindowsPath;
   const DWORD Attrib = GetFileAttributesW(FILmangleFileNameForWindows(Path, &WindowsPath));
   if( Attrib == 0xFFFFFFFF ) {
      COL_THROW_WIN32_ERROR_PLAIN("Failed to resolve if '" << FullPath << "' is a directory. ");
   }
   return (bool)((Attrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}
#else
bool FILisDirectory(const COLstring& FullPath) {
   COL_FUNCTION(FILisDirectory);
   COLassertIfMainThread();
   COLstring Path = FullPath;
   FILcorrectPathSeparators(Path);
   FILremovePathSeparator(Path);
   struct FILstat statbuf;
   if (FILstat(Path.c_str(), &statbuf) < 0)
   {
      COL_THROW_STRERROR_PLAIN("Failed to resolve if '" << FullPath << "' is a directory.");
   }
   return !(S_ISDIR(statbuf.st_mode) == 0);
}
#endif

void FILcopyFile(const COLstring& FromFile, const COLstring& ToFile, bool Overwrite){
   COL_FUNCTION(FILcopyFile);
   COL_VAR2(FromFile, ToFile);

   if (!Overwrite && FILfileExists(ToFile)) {
      COL_ERROR_STREAM_PLAIN("Destination file `" << ToFile << "' exists, but Overwrite is unset.", COLerror::PreCondition);
   }
   COLstring Buffer;
   FILreadFile(FromFile, &Buffer);
   FILwriteFile(ToFile, Buffer);
}


#ifdef _WIN32
void FILrenameFile(const COLstring& FromFile, const COLstring& ToFile){
   COL_FUNCTION(FILrenameFile);
   COL_VAR2(FromFile, ToFile);
   COLstring WinFrom, WinTo;
   if ( 0 != _wrename(FILmangleFileNameForWindows(FromFile, &WinFrom), FILmangleFileNameForWindows(ToFile, &WinTo))) {
      COL_THROW_STRERROR_PLAIN("Attempt to rename '"  << FromFile << "' to '" << ToFile << "' failed.");
   }
}
#else
void FILrenameFile(const COLstring& FromFile, const COLstring& ToFile){
   COL_FUNCTION(FILrenameFile);
   COLassertIfMainThread();
   COL_VAR2(FromFile, ToFile);
   if (0 != ::rename(FromFile.c_str(), ToFile.c_str())) {
      COL_THROW_STRERROR_PLAIN("Attempt to rename '" << FromFile << "' to '" << ToFile << "' failed.");
   }
}
#endif

int FILdirectoryDescentAction(const COLstring& SourceDirPath, const COLstring& DestDirPath, const COLint32 Mode, bool IncludeHidden,
                              COLclosure2 <const COLstring&, const COLstring&>* pFileAction){
   COL_FUNCTION(FILdirectoryDescentAction);
   int Count = 0;
   COL_VAR4(SourceDirPath, DestDirPath, Mode,IncludeHidden);
   if( ! FILfileExists(DestDirPath) ){
      FILmakeFullDir(DestDirPath, Mode);
   }
   COLarray<COLstring> Patterns;
   #ifdef _WIN32
      Patterns.push_back("*.*");
   #else
      Patterns.push_back("*");

      if(IncludeHidden){
         Patterns.push_back(".*");
      }
   #endif
   const bool WantSort        = false; // unsorted is faster
   const bool DirectoriesOnly = false;
   COLstring Pattern;
   for(int i = 0; i < Patterns.size(); i++){
      Pattern = Patterns[i];
      FILdirEnumerator FileWalker(SourceDirPath + FIL_DIR_SEPARATOR + Pattern, DirectoriesOnly, WantSort);
      COLstring CurrentFileName;
      COLstring Source;
      COLstring Dest;
      COL_TRC("Iterating over files in directory with file detection pattern: " << Pattern);
      while( FileWalker.getFile(CurrentFileName) ){
         COL_VAR(CurrentFileName);
         Source = SourceDirPath + FIL_DIR_SEPARATOR + CurrentFileName;
         Dest   = DestDirPath   + FIL_DIR_SEPARATOR + CurrentFileName;
         COL_VAR2(Source, Dest);
         if( FileWalker.isDirectory() ){
            COL_TRC("Found a sub directory, recursing into it...");
            Count += FILdirectoryDescentAction(Source, Dest, Mode, IncludeHidden, pFileAction);
         }
         else{
            COL_TRC("Found a file...");
            pFileAction->run(Source, Dest);
            Count++;
         }
      }
   }
   return Count;
}

int FILrenameDirectory(const COLstring& SourceDirPath, const COLstring& DestDirPath, const COLint32 Mode, bool IncludeHidden){
   COL_FUNCTION(FILrenameDirectory);
   COLclosure2 <const COLstring&, const COLstring&>* pRenameFile = COLnewClosure2(FILrenameFile);
   // must write unit tests
   int Count;
   try{
      Count = FILdirectoryDescentAction(SourceDirPath, DestDirPath, Mode, IncludeHidden, pRenameFile);
      FILremoveFullDirNew(SourceDirPath);
   } catch(const COLerror& Error){
      COL_TRC(Error.description());
      Count = -1;
   }
   delete pRenameFile;
   return Count;
}

int FILcopyDirectoryContents(const COLstring& SourceDirPath, const COLstring& DestDirPath, const COLint32 Mode, bool IncludeHidden){
   COL_FUNCTION(FILcopyDirectoryContents);
   COLclosure2 <const COLstring&, const COLstring&>* pCopyFile = COLnewClosure2(FILcopyFile, true);
   int Count;
   try{
      Count = FILdirectoryDescentAction(SourceDirPath, DestDirPath, Mode, IncludeHidden, pCopyFile);
   } catch(const COLerror& Error){
      COL_TRC(Error.description());
      Count = -1;
   }
   delete pCopyFile;
   return Count;
}

void FILinitExecutableNameImp(const COLstring& OriginalExecutableFilePath, COLstring* pExeFilename, COLstring* pExeDir){
   COL_FUNCTION(FILinitExecutableName);
   COL_VAR(OriginalExecutableFilePath);
   if (FILworkingDirExists()){
      COL_VAR(FILworkingDir());
      *pExeFilename = FILpathResolve(OriginalExecutableFilePath, FILworkingDir());
   } else {
      // IX-2594 this makes the code more robust if the working directory doesn't exist - odd case
      COL_TRC("Working directory does not exist - must be a deleted directory and been given an absolute path");
      COLstring Name, NameErr;
      FILpathSimplify(OriginalExecutableFilePath, &Name, &NameErr);
      *pExeFilename = Name;
   }
   COL_VAR(*pExeFilename);
   *pExeDir = FILpathDir(*pExeFilename);
   FILaddPathSeparator(*pExeDir);
   *pExeFilename = FILpathNameWithoutLastExt(*pExeFilename);
   COL_VAR2(*pExeDir, *pExeFilename);
}

void FILinitExecutableName(const COLstring& OriginalExecutableFilePath){
   COL_FUNCTION(FILinitExecutableName);
   FILinitExecutableNameImp(OriginalExecutableFilePath, &s_ExecutableFileName, &s_ExecutableFileDirectory);
}

COLstring FILexecutableDir(){
   COL_FUNCTION(FILexecutableDir);
   if(s_ExecutableFileDirectory.is_null()) { COL_ERROR_STREAM("Must call FILinitExecutableName(argv[(0]); to use this function", 0); }
   COL_VAR(s_ExecutableFileDirectory);
   return s_ExecutableFileDirectory;
}

COLstring FILexecutableFileName(bool WithExt) {
   COL_FUNCTION(FILexecutableFileName);
   #ifndef _WIN32
       WithExt = false;
   #endif
   COL_VAR(WithExt);
   if (s_ExecutableFileName.is_null()) { COL_ERROR_STREAM("Must call FILinitExecutableName(argv[(0]); to use this function", 0); }
   return WithExt ? s_ExecutableFileName + ".exe" : s_ExecutableFileName;
}

COLstring FILtempDir(){
   COL_FUNCTION(FILtempDir);
   COLassertIfMainThread();
   COLstring TemporaryDirectory;
#ifdef _WIN32
   TemporaryDirectory.assign(MAX_PATH, '\0');
   DWORD CountOfChar = ::GetTempPath(TemporaryDirectory.size(), TemporaryDirectory.get_buffer());
   if (0 == CountOfChar){
      COL_THROW_WIN32_ERROR("GetTempPath failed.");
   }
   TemporaryDirectory = TemporaryDirectory.substr(0, CountOfChar);
#else // not _WIN32
   if (NULL != getenv("TMPDIR")){
      TemporaryDirectory = getenv("TMPDIR");
   }
   if (NULL != getenv("HOME")){
      TemporaryDirectory = getenv("HOME");
      FILaddPathSeparator(TemporaryDirectory);
      TemporaryDirectory += COL_T("tmp");
   }
#endif
   FILaddPathSeparator(TemporaryDirectory);
   if (!FILfileExists(TemporaryDirectory)){
      FILmakeFullDir(TemporaryDirectory, FIL_USER_RWX);
   }
   return TemporaryDirectory;
}

#ifdef _WIN32
// Get the system directory
COLstring FILsystemDir()
{
   COLstring SystemDir;
   SystemDir.assign(MAX_PATH, '\0');
   UINT Result = GetSystemDirectory(SystemDir.get_buffer(), SystemDir.size());
   if (Result > SystemDir.size()){
      SystemDir.assign(Result, '\0');
      Result = GetSystemDirectory(SystemDir.get_buffer(), SystemDir.size());
   }
   if (0 == Result){ // precondition
      COL_THROW_WIN32_ERROR("GetSystemDirectory failed.");
   }
   SystemDir = SystemDir.substr(0, Result);
   FILaddPathSeparator(SystemDir);
   return SystemDir;
}

// Get the windows directory
COLstring FILwindowsDir()
{
   COLstring WindowDir;
   // WindowDir = "";
   UINT Result = GetWindowsDirectory(WindowDir.get_buffer(), WindowDir.size());
   if (Result > WindowDir.size()){
      WindowDir.assign(Result, '\0');
      Result = GetWindowsDirectory(WindowDir.get_buffer(), WindowDir.size());
   }
   if (0 == Result){
      COL_THROW_WIN32_ERROR("GetWindowsDirectory failed.");
   }
   WindowDir = WindowDir.substr(0, Result);
   FILaddPathSeparator(WindowDir);
   return WindowDir;
}

wchar_t* FILmangleFileNameForWindows(const COLstring& FileName, COLstring* pMangled) {
   COL_FUNCTION(FILmangleFileNameForWindows);
   COLstring& Mangled = *pMangled;
   int WCharCount = ::MultiByteToWideChar(CP_UTF8, 0, FileName.c_str(), -1, NULL, 0);
   int Size = sizeof(wchar_t) * WCharCount;
   Mangled.assign(Size, '\0');
   wchar_t* pMangledWide = (wchar_t*)Mangled.get_buffer();
   ::MultiByteToWideChar(CP_UTF8, 0, FileName.c_str(), FileName.size(), pMangledWide, WCharCount);
   COL_VAR(FileName);
   COL_DUMP("Mangled filename:", Mangled.c_str(), Mangled.size());
   return pMangledWide;
}

void FILunMangleWindowsFileName(const wchar_t* pMangled, COLstring* pFixed) {
   COL_FUNCTION(FILunMangleWindowsFileName);
   COLstring& Fixed = *pFixed;
   int ByteCount = ::WideCharToMultiByte(CP_UTF8, 0, pMangled, -1, NULL, 0, NULL, NULL);
   COL_VAR(ByteCount);
   Fixed.assign(ByteCount, '\0');
   ::WideCharToMultiByte(CP_UTF8, 0, pMangled, -1, Fixed.get_buffer(), ByteCount, NULL, NULL);
   Fixed.setSize(ByteCount - 1); // Makes the null byte included in the string a terminator instead.
   COLvar Check = Fixed;
   COL_VAR(Check.json(true)); // Byte view
   COL_VAR(Check.json(true, COLvar::StrEncJs)); // Friendly view
}

void FILremoveWithThrow(const COLstring& FileName, bool RemoveReadOnly){
   COL_FUNCTION(FILremoveWithThrow);
   COLstring WindowsName;
   wchar_t* pReadyToUse = FILmangleFileNameForWindows(FileName, &WindowsName);
   COL_VAR3(FileName, RemoveReadOnly, pReadyToUse);
   if(RemoveReadOnly && (GetFileAttributesW(pReadyToUse) & FILE_ATTRIBUTE_READONLY)){
      COL_TRC("Removing the READONLY attribute so file can be deleted.");
      SetFileAttributesW(pReadyToUse, FILE_ATTRIBUTE_NORMAL);
   }
   if(GetFileAttributesW(pReadyToUse) & FILE_ATTRIBUTE_HIDDEN){
      COL_TRC("Removing the HIDDEN attribute so file can be deleted.");
      SetFileAttributesW(pReadyToUse, FILE_ATTRIBUTE_NORMAL);
   }
   if (-1 == _wremove(pReadyToUse)) {
      long LastError = COLlastError();
      COLstring Description = COLerrorString(LastError);
      COL_VAR2(LastError, Description);
      throw COLerror("Attempt to remove '" + FileName + "' failed: " + Description, LastError);
   }

}

int FILremove(const COLstring& FileName, bool RemoveReadOnly) {
   COL_FUNCTION(FILremove);
   COLstring WindowsName;
   wchar_t* pReadyToUse = FILmangleFileNameForWindows(FileName, &WindowsName);
   COL_VAR3(FileName, RemoveReadOnly, pReadyToUse);
   if(RemoveReadOnly && (GetFileAttributesW(pReadyToUse) & FILE_ATTRIBUTE_READONLY)){
      COL_TRC("Removing the READONLY attribute so file can be deleted.");
      SetFileAttributesW(pReadyToUse, FILE_ATTRIBUTE_NORMAL);
   }
   if(GetFileAttributesW(pReadyToUse) & FILE_ATTRIBUTE_HIDDEN){
      COL_TRC("Removing the HIDDEN attribute so file can be deleted.");
      SetFileAttributesW(pReadyToUse, FILE_ATTRIBUTE_NORMAL);
   }
   if (-1 == _wremove(pReadyToUse)) {
      long LastError = COLlastError();
      COL_VAR2(LastError, COLerrorString(LastError));
      return LastError;
   }
   return 0;
}

void FILremoveDir(const COLstring& DirectoryName, bool RemoveReadOnly) {
   COL_FUNCTION(FILremoveDir);
   COLstring WindowsDirName;
   wchar_t* pWindowsDir = FILmangleFileNameForWindows(DirectoryName, &WindowsDirName);
   if (RemoveReadOnly && (GetFileAttributesW(pWindowsDir) & FILE_ATTRIBUTE_READONLY)){
      COL_TRC("Removing the READONLY attribute so dir can be deleted.");
      SetFileAttributesW(pWindowsDir, FILE_ATTRIBUTE_NORMAL);
   }
   if (GetFileAttributesW(pWindowsDir) & FILE_ATTRIBUTE_HIDDEN){
      COL_TRC("Removing the HIDDEN attribute so dir can be deleted.");
      SetFileAttributesW(pWindowsDir, FILE_ATTRIBUTE_NORMAL);
   }
   if (-1 == _wrmdir(pWindowsDir)) {
      COL_THROW_STRERROR_PLAIN("Attempt to remove directory '" << DirectoryName << "' failed.");
   }
}
#else
// POSIX implementation
void FILremoveWithThrow(const COLstring& FileName, bool RemoveReadOnly) {
   COL_FUNCTION(FILremoveWithThrow);
   COLassertIfMainThread();
   if (-1 == remove(FileName.c_str())) {
      COL_THROW_STRERROR_PLAIN("Attempt to remove '" << FileName << "' failed.");
   }
}

// error code. POSIX implementation
int FILremove(const COLstring& FileName, bool RemoveReadOnly){
   COL_FUNCTION(FILremove);
   COLassertIfMainThread();
   if (-1 == remove(FileName.c_str())) {
      long Err = COLlastError();
      COL_VAR2(Err, COLerrorString(Err));
      return Err;
   }
   return 0;
}

void FILremoveDir(const COLstring& DirectoryName, bool RemoveReadOnly) {
   COL_FUNCTION(FILremoveDir);
   COLassertIfMainThread();
   if (-1 == FILrmdir(DirectoryName.c_str())) {
      COL_THROW_STRERROR_PLAIN("Attempt to remove directory '" << DirectoryName << "' failed.");
   }
}
#endif

void FILdeleteFileIfExists(const COLstring& FilePath, bool RemoveReadOnly) {
   COL_FUNCTION(FILdeleteFileIfExists);
   if (!FilePath.is_null() && FILfileExists(FilePath)){
      FILremoveWithThrow(FilePath, RemoveReadOnly);
   }
}

bool FILisSymbolicLink(const COLstring& FileName){
   COLassertIfMainThread();
#ifdef _WIN32
   return false;
#else
   struct stat FileStat;
   if (-1 == lstat(FileName.c_str(), &FileStat)){
      COL_THROW_STRERROR_PLAIN("Failed to resolve if  " << FileName << " is a symbolic link.");
   }
   return ((FileStat.st_mode & S_IFLNK) == S_IFLNK);
#endif
}

void FILdirList(COLlist<COLstring>* pList, const COLstring& Wildcard, bool DirsOnly, bool WantSort){
   COL_FUNCTION(FILdirList);
   FILdirEnumerator DirEnumerator(Wildcard, DirsOnly, WantSort);
   COLstring Dir;
   while (DirEnumerator.getFile(Dir)){
      pList->push_back(Dir);
   }
}

void FILdirList(COLmap<COLstring, bool,  COLmapCompare<COLstring> >* pList, const COLstring& Wildcard){
   COL_FUNCTION(FILdirList);
   FILdirEnumerator Enum(Wildcard, false, true);
   COLstring FileName;
   while (Enum.getFile(FileName)){
      (*pList)[FileName] = Enum.isDirectory();
   }
}

#ifdef _WIN32
COLstring FILcurrentUser(){
   COL_FUNCTION(FILcurrentUser);
   COLstring UserName;
   UserName.setCapacity(256);
   DWORD Length=256;
   BOOL Result = GetUserNameA((LPSTR)UserName.get_buffer(), &Length);
   COL_VAR(Length);
   UserName.setSize(Length-1); // -1 since Length includes null terminating character
   COL_VAR(UserName);
   return UserName;
}
#else // POSIX
COLstring FILcurrentUser(){
   COL_FUNCTION(FILcurrentUser);
   COLstring UserName;
   uid_t uid = getuid();
   struct passwd* pInfo = getpwuid(uid);
   if (pInfo) {
      return pInfo->pw_name;
   }
   COL_VAR(UserName);
   return UserName;
}

#endif // end of POSIX block

bool FILsave(const COLstring& File, const COLstring& Data, COLstring* pErr) {
	COL_FUNCTION(FILsave);
	try {
		if(!FILfileExists(File)) { FILcreateDirectoryTreeToFile(File, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX); }
		FILwriteFile(File, Data);
		return true;
	} catch(const COLerror& E) {
		*pErr = E.description();
		return false;
	}
}

static COLstring FILreservedCharacters() {
   static COLstring ReservedCharacters("<>:\"/\\|?*");
   return ReservedCharacters;
}

static bool FILnameContainsReservedCharacters(const COLstring& Name) {
   for (int i = 0; i < Name.size(); ++i) {
      switch (Name[i]) {
         case '<':
         case '>':
         case ':':
         case '"':
         case '/':
         case '\\':
         case '|':
         case '?':
         case '*':   return true;
         default:    break;
      }
   }
   return false;
}

static COLstring FILreservedNames() {
   static COLstring ReservedNames("CON, PRN, AUX, NUL, COM1, COM2, COM3, COM4, COM5, COM6, COM7, COM8, COM9, LPT1, LPT2, LPT3, LPT4, LPT5, LPT6, LPT7, LPT8, and LPT9");
   return ReservedNames;
}

static bool FILnameMatchesReservedNames(const COLstring& Name) {
   if (Name.size() == 3) {
      // CON, PRN, AUX, NUL
      return (Name == "CON") || (Name == "PRN") || (Name == "AUX") || (Name == "NUL");
   }
   if (Name.size() == 4) {
      // COM1, COM2, COM3, COM4, COM5, COM6, COM7, COM8, COM9
      // LPT1, LPT2, LPT3, LPT4, LPT5, LPT6, LPT7, LPT8, LPT9
      if (memcmp(Name.data(), "COM", 3) == 0 || memcmp(Name.data(), "LPT", 3) == 0) {
         return 48 < Name[3] && Name[3] < 58;
      }
      return false;
   }
   return false;
}

static bool FILnameEndsWithDot(const COLstring& Name) {
   if (Name.is_null()) { return false; }
   return Name[Name.size() - 1] == '.';
}

bool FILnameIsInvalid(const COLstring& Name, COLstring* pError) {
   COL_FUNCTION(FILnameIsInvalid);
   if (Name.is_null()) { return true; }
   if (Name[0] == ' ' || Name[Name.size() - 1] == ' ') {
      if (pError) { *pError = "'" + Name + "' starts or ends with spaces."; }
      return true;
   }
   if (FILnameContainsReservedCharacters(Name)) {
      if (pError) { *pError = "'" + Name + "' contains reserved characters " + FILreservedCharacters() + "."; }
      return true;
   }
   if (FILnameMatchesReservedNames(Name)) {
      if (pError) { *pError = "'" + Name + "' is a reserved name " + FILreservedNames() + "."; }
      return true;
   }
   if (FILnameEndsWithDot(Name)) {
      if (pError) { *pError = "'" + Name + "' ends with a period."; }
      return true;
   }
   return false;
}

bool FILpathIsInvalid(const COLstring& Path, COLstring* pError) {
   COLarray<COLstring> Parts;
   COLsplit(&Parts, Path, FIL_DIR_SEPARATOR);
   for (const auto& Part : Parts) {
      if (Part.is_null()) { continue; }
      if (FILnameIsInvalid(Part, pError)) { return true; }
   }
   return false;
}
