#ifndef __PRO_UTILS_H__
#define __PRO_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROutils
//
// Description:
//
// Set of utilities for the PRO library
//
// Author: Rishi Marwah, Monica Shver, Eliot Muir
// Date:   Tue 09/01/1998 
//---------------------------------------------------------------------------

class COLstring;
class COLclosure0;
template<typename T> class COLarray;
template<typename T> class PROptrArray;

bool PROisProcessRunning(int ProcessID);

// Use this to initialize tracing from an environmental variable - useful for things like CHM_LIB3.dll
void PROtraceInit(const COLstring& Env);

// Get a callback for when we get control C event.
// Windows implementation registers for a ctrl-break or ctrl-c signal on construction
// Unix implementation registers for a SIGTERM signal on construction
// If you pass NULL it will unregister the callback.
void PROsetOnKill(COLclosure0* pOnCloseEvent);


// Parse a platform-specific command-line into the unescaped arguments.
//
// On Windows, PROparseWindowsCommandLine is used.
// On Posix,   PPROparsePosixCommandLine is used.
void PROparseCommandLine(const COLstring& CommandLine, COLarray<COLstring>* pArgumentsOut);

// Build a platform-specific command-line from a set of unescaped arguments.
//
// On Windows, PRObuildWindowsCommandLine is used.
// On Posix,   PRObuildPosixCommandLine is used.
COLstring PRObuildCommandLine(const COLarray<COLstring>& Arguments);

// Parse a Posix command-line into the unescaped arguments.
// The command, if present, will be returned as the first argument.
//
// - White-space delimits arguments, except when escaped or quoted.
// - Double-quotes are used for quoting.  Escape characters and whitespace
//   inside a quoted segment are treated literally, except for escaped
//   double-quotes and escaped escape character.  E.g.: "\ \\ \"" => \ \ ".
// - Backslash is the escape character.  Any character following the escape
//   character is taken literally.  Whitespace may be escaped, making use of
//   quoting entirely optional.  E.g. "a b" == a\ b.  Quoting looks prettier!
void PROparsePosixCommandLine(const COLstring& CommandLine, COLarray<COLstring>* pArgumentsOut);

// Build a Posix command-line from a set of unescaped arguments.
// Include the command as the first argument to build a complete command-line.
COLstring PRObuildPosixCommandLine(const COLarray<COLstring>& Arguments);

// Build an argv array from a set of unescaped arguments, suitable for Posix execvp.
//
// The first argument must be the command.  Argv[0] will be set to point to the command
// filename, excluding its path (if present).

// Warning: pArgvArrayOut will point to the strings inside Arguments (e.g. there is no
// additional string allocation done). Hence the lifetime of Arguments must be greater
// than that of *pArgvArrayOut.
void PRObuildPosixArgvArray(const COLarray<COLstring>& Arguments, PROptrArray<char> * pArgvArrayOut);

// Parse a Windows command-line into the unescaped arguments.
// The command, if present, will be returned as the first argument.
//
// From MSDN C++ Language Reference "Parsing C++ Command-Line Arguments":
//
// - Arguments are delimited by white space, which is either a space or a tab.
// - A string surrounded by double quotation marks ("string") is interpreted as a
//   single argument, regardless of white space contained within. A quoted string can
//   be embedded in an argument.
// - A double quotation mark preceded by a backslash (\") is interpreted as a literal
//   double quotation mark character (").
// - Backslashes are interpreted literally, unless they immediately precede a double
//   quotation mark.
// - If an even number of backslashes is followed by a double quotation mark, one
//   backslash is placed in the argv array for every pair of backslashes, and the
//   double quotation mark is interpreted as a string delimiter.
// - If an odd number of backslashes is followed by a double quotation mark, one
//   backslash is placed in the argv array for every pair of backslashes, and the double
//   quotation mark is "escaped" by the remaining backslash, causing a literal double
//   quotation mark (") to be placed in argv.
void PROparseWindowsCommandLine(const COLstring& CommandLine,COLarray<COLstring>* pArgumentsOut);

// Build a Windows command-line from a set of unescaped arguments.  This command-line
// is suitable for passing to Win32's CreateProcess, but not to CMD.EXE or anywhere else
// that has special characters (e.g. |<>$*...).
//
// Include the command as the first argument to build a complete command-line.
COLstring PRObuildWindowsCommandLine(const COLarray<COLstring>& Arguments);

int PROpid();

#ifndef _WIN32
COLstring PROescapePosixCommandLineArgument(const COLstring& Argument);
#endif

#endif // end of defensive include

