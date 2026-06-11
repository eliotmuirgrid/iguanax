#ifndef __CFG_CONFIG_API_H__
#define __CFG_CONFIG_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CFGconfigApi
//
// Description:
//
// Helper functions which operate on the CFGconfig object.
//
// General idea is to separate the concern of how we load the CFGconfig object
// from the operations we need to perform on it.
//
// So on worker threads for instance we can load the CFGconfig object from a file on a blocking
// file IO call and on event loop we can pull CFGconfig objects from the CFGmap collection
//
// Author: Eliot Muir
// Date:   Mon 11 Jul 2022 16:09:58 EDT
//---------------------------------------------------------------------------

struct CFGconfig;
class COLstring;
class COLvar;

bool CFGdevelopmentMode(const COLstring& CommitId, bool ForceDevelopment);

// used for various api's that pass in component id as well as development flag
// It will return an EMPTY string if the relative path goes above the root, or the component does not exist
COLstring CFGresolveRepoFile(const COLstring& Component, const COLstring& RelativeFilePath, bool IsDevelopment);

#endif // end of defensive include
