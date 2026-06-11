#ifndef __FIL_OS_DEFS_H__
#define __FIL_OS_DEFS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILosDefs
//
// Description:
//
// This file has defines that unify POSIX and _WIN32
//
// Author: Eliot Muir
// Date:   Fri 01/16/2004
//---------------------------------------------------------------------------

#include <errno.h>      // IO function errors
#include <fcntl.h>      // open flags for Windows and most IO functions for POSIX
#include <sys/types.h>  // stat APIs
#include <sys/stat.h>

#ifdef _WIN32
#include <io.h>         // all IO functions for Windows
#include <direct.h>     // _getcwd, _chdir, _rmdir

#   define FILlseek           _lseek
#   define FILlseek64         _lseeki64
#   define FILopen            _open
#   define FILfileno          _fileno
#   define FILclose           _close
#   define FILwrite           _write
#   define FILread            _read
#   define FILcommit          _commit
#   define FILeof             _eof
#   define getcwd             _getcwd
#   define FILstat            _stat
#   define FILwstat           _wstat
#   define FILaccess          _access
#   define FILchdir           _chdir
#   define FILrmdir           _rmdir

#   define FILexistsPermissions 0

#else // POSIX
#include <unistd.h>     // close, fsync

#   define O_BINARY     0   // windows FILopen flag
#   define O_NOINHERIT  0   // windows FILopen flag, prevents delete permission issues
#   define FILlseek           ::lseek
#   define FILlseek64         ::lseek
#   define FILopen            ::open
#   define FILfileno          ::fileno
#   define FILclose           ::close
#   define FILwrite           ::write
#   define FILread            ::read
#   define FILcommit          ::fsync
#   define FILstat            ::stat
#   define FILaccess          ::access
#   define FILchdir           ::chdir
#   define FILrmdir           ::rmdir

#   define _MAX_PATH 2048
#   define FILexistsPermissions F_OK

#endif // End of POSIX


#endif // end of defensive include
