#ifndef __FIL_UTILS_H__
#define __FIL_UTILS_H__
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
#include <COL/COLstring.h>
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLlist.h>
#include <time.h>

template<class K>
class COLmapCompare;

template <typename K, typename V, class CompareType>
class COLmap;

// standard file permissions (R=read, W=write, X=execute)

extern const COLuint32 FIL_READABLE;
extern const COLuint32 FIL_WRITABLE;

extern const COLuint32 FIL_MAGIC_NUMBER;

#ifndef _WIN32
extern const COLuint32 FIL_EXECUTABLE;

#   ifndef DEFFILEMODE
#      define DEFFILEMODE S_IRUSR | S_IWUSR |S_IRGRP | S_IROTH
#   endif

#endif

#define FIL_USER_RWX 0x01C0
#define FIL_GROUP_RX 0x0028
#define FIL_OTHER_RX 0x0005

class COLstring;
class COLsink;

typedef COLauto< COLclosure2<const COLstring& /* In */, COLstring* /* pOut */> > FILnameFilter;

// This one throws exceptions.
void FILreadFile(const COLstring& FileName, COLstring* pDestination);
void FILwriteFile(const COLstring& FileName, const COLstring& Data);
void FILappendFile(const COLstring& FileName, const COLstring& Data);


void FILaddPathSeparator(COLstring& Path);              // append '\' if necessary
void FILremovePathSeparator(COLstring& Path);           // remove '\' if necessary
void FILpathSepLeadingRemoveOneNative(COLstring& Path); // remove '\' from start of path if necessary

// Change all separators in a string so that they conform to the platforms separator specifications (ex. convert / to \ on win)
void FILcorrectPathSeparators(COLstring& Path);

// Note that this function used to return a Boolean and now throws an exception when it fails.
void FILsetWorkingDir(const COLstring& Dir);

time_t FILmodified(const COLstring& FileName);

// Note that this function used to return a Boolean and now throws an exception when it fails.  Mode is only applicable under
// Posix - it's ignored under Windows.  It specifies the permissions to use when creating the directory.
void FILmakeDir(const COLstring& Dir, const COLint32 Mode, const char* pOwnerUserName = NULL);

//Copy a SourceDir's contents (including sub directories and their files),
//into another folder. The DestDir doesn't have to exist for this to work.
int FILcopyDirectoryContents(const COLstring& SourceDirPath, const COLstring& DestDirPath, const COLint32 Mode, bool IncludeHidden=true);

// This function uses the FILfilePath object to parse the DirPath argument and will make
// sure that all of the directories in the path exist
// TODO - need to Test this function
void FILmakeFullDir(const COLstring& DirPath, const COLint32 Mode, const char* pOwnerUserName = NULL);

// Given a path to a file, will make sure that all directories leading up to the file have been created.
// Unlike FILmakeFullDir which requires the path to end in a directory.
void FILcreateDirectoryTreeToFile(const COLstring& FilePath, const COLint32 Mode);

// Returns the current working directory - always with one directory separator
// appended to the end.  This is slightly different behaviour from earlier versions
COLstring FILworkingDir();

// Returns true or false depending on whether the file is a directory
// WARNING: will throw an exception if the attribute check fails
// e.g. when the path doesn't exist
// - check for FILfileExists() first if this might be the case
bool FILisDirectory(const COLstring& FullPath);

bool FILisDirectoryEmpty(const COLstring& DirectoryName);

// if Overwrite == false and the file exists, an exception is thrown.
// destination directory must exist; if you want to copy to a dirtree that doesn't
// exist, use FILmakeFullDir before you call FILcopyFile.
void FILcopyFile(const COLstring& FromFile, const COLstring& ToFile, bool Overwrite = true);

// I removed FILmoveFile since it didn't follow the semantics of the rest
// of the FILutils library - i.e. throwing exceptions when errors occur - use
// this function instead  (FILrename has been removed too )
void FILrenameFile(const COLstring& FromFile, const COLstring& ToFile);

int FILrenameDirectory(const COLstring& SourceDirPath, const COLstring& DestDirPath, const COLint32 Mode, bool IncludeHidden);

// This function is only implemented under POSIX
bool FILisSymbolicLink(const COLstring& FileName);

// Call FILinitExecutableName(argv[0]); before calling FILexecutableDir or FILexecutableFileName
void FILinitExecutableName(const COLstring& ExecutableFilePath);

// Get the directory of the current application
COLstring FILexecutableDir();

// File name only of the current application - i.e. iguana or iguana.exe - WithExt is ignored in POSIX
COLstring FILexecutableFileName(bool WithExt = true);

#ifdef _WIN32
// Get the system directory
COLstring FILsystemDir();
// Get the windows directory
COLstring FILwindowsDir();
wchar_t* FILmangleFileNameForWindows(const COLstring& FileName, COLstring* pMangled);
void FILunMangleWindowsFileName(const wchar_t* pMangled, COLstring* pFixed);
#endif // !_WIN32

bool FILfileExists(const COLstring& FileName);
bool FILfileExists(const COLstring& FileName, FILnameFilter pFilter);
bool FILfileReadWritable(const COLstring& FileName);

bool FILexecutableExists(const COLstring& CommandLine);
bool FILhasPermission(const COLstring& FileName, COLuint32 PermissionMode);

// POSIX only - null op on windows
void FILsetPermission(const COLstring& FileName, COLuint32 PermissionMode);

// Warning - don't use this on an Open file - it is not reliable
// in that situation - use the .size() method on FILfile instead
// in those cases.
COLuint64 FILfileSize(const COLstring& Filename);

COLstring FILtempDir();

#ifdef _WIN32
#  define FIL_DIR_SEPARATOR "\\"
#  define FIL_PATH_SEPARATOR ";"
#  define FIL_NEWLINE_CHARACTER "\r\n"
#else
#  define FIL_DIR_SEPARATOR "/"
#  define FIL_PATH_SEPARATOR ":"
#  define FIL_NEWLINE_CHARACTER "\n"
#endif

#define FIL_POSIX_DIR_SEPARATOR "/"
#define FIL_POSIX_PATH_SEPARATOR ":"


// wrapper for the remove() library call
// FIXME WARNING RED FLAG - it has inconsistent behavior between windows and posix
// Windows: This function will fail to remove a directory - use FILremoveDir instead
//    ref http://msdn.microsoft.com/en-us/library/2da4hk1d(v=vs.90).aspx
//    > ENOENT to indicate .. or that the path specifies a directory.
// Posix: remove() deletes a name from the filesystem.  It calls unlink(2) for files, and rmdir(2) for directories.
//    ref man remove
//
// On Windows passing true to the RemoveReadOnly flag will delete files/dirs even if they
// have the read-only attribute set.
void FILremoveWithThrow(const COLstring& FileName, bool RemoveReadOnly = false);

// This version of FILremove will not throw exceptions - but it could fail and return an
// error code.
int FILremove(const COLstring& FileName, bool RemoveReadOnly = false);

// This function will remove a complete directory (including read only/hidden files).
// Done entirely with native system implementations to avoid recursive delete errors (see #26846)
void FILremoveFullDirNew(const COLstring& DirectoryName);
// This one calls FILremoveFullDirNew but won't throw. Instead will populate pError if the removal failed.
bool FILremoveFullDirNewSafe(const COLstring& DirectoryName, COLstring* pError);

void FILdeleteFileIfExists(const COLstring& FilePath, bool RemoveReadOnly = false);

void FILremoveDir(const COLstring& DirectoryName, bool RemoveReadOnly=false);

// This function lists files/dirs matching the wildcard.  Under the hood it uses FILdirEnumerator
// It's intended to be a more convenient interface to getting a list of files in one go rather than
// having to iterate them through FILdirEnumerator
void FILdirList(COLlist<COLstring>* pList, const COLstring& Wildcard, bool DirsOnly=false, bool WantSort=false);

// Eliot the linked list version above was for when I wrote the code to iterate through the PDB files.
// This one uses a map and the true value value is for whether we are dealing with a directory or not.
void FILdirList(COLmap<COLstring, bool, COLmapCompare<COLstring> >* pList, const COLstring& Wildcard);

// Get the current user - works cross plaform
COLstring FILcurrentUser();

bool FILsave(const COLstring& File, const COLstring& Data, COLstring* pErr);

// Check if the filename is a reserved name or contains reserved characters
bool FILnameIsInvalid(const COLstring& Name, COLstring* pError = nullptr);
// Check if the path contains an invalid name
bool FILpathIsInvalid(const COLstring& Path, COLstring* pError = nullptr);

#endif // end of defensive include
