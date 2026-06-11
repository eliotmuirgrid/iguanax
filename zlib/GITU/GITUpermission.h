#ifndef __GITU_PERMISSION_H__
#define __GITU_PERMISSION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUpermission
//
// Description:
//
// Code to see if we have permissions for a given repo.
// Deliberately using the GITUurl address for them for a normalized addressing scheme.
//
// Author: Eliot Muir 
// Date:   Sunday 26 March 2023 - 05:21PM
// ---------------------------------------------------------------------------

class COLstring;

// Can we write to the repository 
// ScratchDir must be an absolute path - should be a unique directory under the user's configuration directory
// SkipUrlNormalization should be set to true for Azure urls.
bool GITUpermissionWrite(const COLstring& Username, const COLstring& Url, const COLstring& ScratchDir, int* pExitCode, COLstring* pError);

// Can we read to the repository 
bool GITUpermissionRead(const COLstring& Url);


#endif // end of defensive include
