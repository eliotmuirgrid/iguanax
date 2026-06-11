#ifndef __FIL_PATH_UTILS_H__
#define __FIL_PATH_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILpathUtils
//
// Description:
//
// A replacement for the old FILfilePath class.
//
// Author: Matt Lawrence
// Date:   Monday, April 6th, 2009 @ 02:53:09 PM
//---------------------------------------------------------------------------

#include <COL/COLstring.h>

template<class T> class COLarray;

// Simplifies the given path by removing the following:
//  * Excess separators, with the exception of those at the start of a samba
//    share ('\\')
//  * './' in the path
//  * '../' are removed if possible (i.e. FILsimplifyPath("..") = "..")
// Returns false with error message in pErr if Path has > 256 directories. 
bool FILpathSimplify(const COLstring& Path, COLstring* pOut, COLstring* pErr);
// Exposed only for unit tests.
COLstring FILpathSimplifyUnsafe(const COLstring& Path);

// Cuts sequences of dots and slashes down to single characters, and removes
// leading dots and slashes. Mostly used to clean filenames typed by hand.
// E.g. .././..\/. -> .
// E.g. /////////////../../ -> /
// E.g. /fun.file -> fun.file
COLstring FILpathUnCruft(const COLstring& Path);

// Attempts to resolve a full path from the given relative one, using
// WorkingDirectory as the base. If Path is already absolute, then it is
// simplified and has its separators corrected. Otherwise it is appended to 
// WorkingDirectory and then simplified/corrected. If WorkingDirectory is 
// empty, then FILworkingDir() is used to determine the base path.
COLstring FILpathResolve(const COLstring& Path, COLstring WorkingDirectory = COLstring());

// Appends PathB to PathA, adding and correcting separators as needed. Also
// will simplify the path by removing things such as './' and '../'. 
COLstring FILpathAppend(const COLstring& PathA, const COLstring& PathB);

// Returns true if Path is an absolute Samba, Windows or *NIX path.
bool FILpathIsAbsolute(const COLstring& Path);

// returns last extension - everything to the right of the last dot (taking into account IncludeDot) - or empty string if dot is not found.
// using with IncludeDot=true is recommended - e.g. FILpathExtLast(FileName,true) == ".cpp"
// usage with IncludeDot=false may lead to funny special cases (filenames that end with dot, or whole filename that looks like an extension)
COLstring FILpathExtLast(const COLstring& FileName, bool IncludeDot);

// Returns the name part, without the last extension (up to the last dot, not including the dot) of the given path
// or the whole name part if no dot exists
COLstring FILpathNameWithoutLastExt(const COLstring& Path);

// Returns the name part, as is, complete with any extensions
// This could be qualified with Full, Complete, Raw etc. but it would raise just as many questions, and short, simple and concise is good, so just Name.
COLstring FILpathName(const COLstring& Path);

// Extracts the directory name without trying to resolve it.
// Unlike FILfilePath::directory, in that it does include the volume name and
// does not try to resolve the path.
COLstring FILpathDir(const COLstring& Path, bool Simplify=true);

// Same as above, but also attempts to resolve the directory if it is relative.
// Similar to FILfilePath::fullDirectory
COLstring FILpathDirResolved(const COLstring& Path, COLstring WorkingDirectory = COLstring());

// Returns the volume name for the path, such as "D:" or "\\share\foo\". If the
// path is not absolute, it will attempt to resolve it based on the specified
// working dir. Returns "" if the path is relative or *NIX style.
COLstring FILpathVolumeResolved(const COLstring& Path, COLstring WorkingDirectory = COLstring());

// foo/bar/zoo/file.ext -> ["foo", "foo/bar", "foo/bar/zoo"]
// c:\foo\bar\zoo\file.exe -> ["c:\foo", "c:\foo\bar", "c:\foo\bar\zoo"]
void FILpathParentDirList(COLarray<COLstring>& DirList, const COLstring& FilePath);

extern const char* FIL_PATH_SEP_POSIX; /* = "/" */
extern const char* FIL_PATH_SEP_WINDOWS; /* = "\\" */
#ifdef _WIN32
#define FIL_PATH_SEP_NATIVE FIL_PATH_SEP_WINDOWS
#else
#define FIL_PATH_SEP_NATIVE FIL_PATH_SEP_POSIX
#endif

// removes any number of trailing separators of either type of slash, and optionally removes trailing dots.
COLstring FILpathSepRemove(const COLstring& Path, bool RemoveDotsToo = false);


// adds a single separator of the correct type, replacing any number of existing separators of either type
COLstring FILpathSepAdd(const COLstring& Path, const char* Sep=FIL_PATH_SEP_NATIVE);

COLstring FILpathSepToPosix(const COLstring& Path);
COLstring FILpathSepToNative(const COLstring& Path);

// This function resolves absolute or relative paths, expanding all environmental variables,
// and returning an absolute path. It uses COLexpandEnvironmentVariables, so the same
// caveats about string literals of form "${VARIABLE}" apply.

COLstring FILpathResolveEnvironment(const COLstring&);

// Return an expanded path of the form ~/some/dir to $HOME/some/dir
// Does not support paths of the form ~username/some/dir
// Returns the same string if cannot expand.
COLstring FILexpand(const COLstring& Path);

// return path to its parent folder
COLstring FILparentDir(COLstring Path);
#endif // end of defensive include
