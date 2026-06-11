#ifndef __INST_PERMISSION_H__
#define __INST_PERMISSION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTpermission
//
// Description: Grant a user 'Full control' of the Iguana working dir so that
//    the IguanaX service could run as the said user.
//    The command version is tested but slow. The API version is significantly
//    faster but requires more testing.
//    See https://interfaceware.atlassian.net/browse/IX-1892 for details.
//
// Author: John Q
// Date:   Mon 14 Aug 2023 07:28:34 PDT
// ---------------------------------------------------------------------------
class COLstring;

// Gives User full control of the directory at DirPath using Windows API.
// Equivalent to `icacls "<DirPath>" /Q /T /grant "<User>":(OI)(CI)F` but ~5 times faster,
// or 20 times faster than `icacls "<DirPath>" /T /grant "<User>":(OI)(CI)F` for a dir with 4k+ files.
bool INSTgrantUserFullDirectoryControl(const COLstring& DirPath, const COLstring& User, COLstring* pError);

// Gives User full control of the directory at DirPath by running an `icacls` command.
// Uses the /Q (quite) option to make it 4 times faster than the verbose version for a dir with 4k+ files.
bool INSTgrantUserFullDirectoryControlOld(const COLstring& DirPath, const COLstring& User, COLstring* pError);


#endif // end of defensive include
