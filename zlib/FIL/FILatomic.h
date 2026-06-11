#ifndef __FIL_ATOMIC_H__
#define __FIL_ATOMIC_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FILatomic
//
// Description:
//
// Methods to atomically save data (in COLstring form) to disk, and to read 
// atomically saved data from disk into a COLstring.
//
// When saving, writes to .temp file before overwriting the provided file.
// When reading, will try to read provided file first. If it doesn't exist, 
// will try to rename and use .temp file if it exists.
//
// Author: Vismay Shah
// Date:   Wed, March 3rd, 2021
//---------------------------------------------------------------------------
class COLstring;
class COLvar;

// TODO - Eliot - I would really like to simplify these interfaces
// should have the same interface as FILwriteFile and FILreadFile.
// Also places in the code use these helper functions when it doesn't
// make sense - like loading a file checked out from a repo.
void FILsaveAtomic(const COLstring& FileName, const COLstring& Content);
bool FILloadAtomic(const COLstring& FileName, COLstring* pData);

// wrappers for saving and loading JSON config files.
void FILserializeSaveConfig(const COLvar& Config, const COLstring& FileName);
bool FILserializeLoadConfig(COLvar* pConfig, const COLstring& FileName);

#endif // end of defensive include