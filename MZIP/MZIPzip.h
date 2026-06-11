#ifndef __MZIP_ZIP_H__
#define __MZIP_ZIP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: MZIPzip
//
// Description:
//
// MZIPzip class.
//
// Author: Ivan Hamer
// Date:   Monday, June 6th, 2011 @ 12:55:35 PM
//---------------------------------------------------------------------------

#include <COL/COLarray.h>
#include <COL/COLstring.h>

#include "MZIPvar.h"
#include "MZIPmemory.h"
#include <MZIP/zip.h>

// The following will create a zip file (ZipFileName) with the list of files (FilesToZip) relative to a given RootDir.
// The RootDir will not be present in the zip file, and is used only to get to the full path of the files to zip.
// RootDir should have directory separator at the end.
// Returns MZIP_SUCCESS or MZIP_FAILURE with Err set to the error string.
int MZIPzipFiles(const COLstring& ZipFileName, const COLarray<COLstring>& FilesToZip, const COLstring& RootDir, COLstring* pErr=0);

// Same inputs as above, but output to pMemoryZip
int MZIPzipFilesToMemory(const COLarray<COLstring>& FilesToZip, const COLstring& RootDir, COLstring* pMemoryZip, COLstring* pErr=0);

voidpf ZCALLBACK win32_open_wide_file_func(voidpf opaque, const char* filename, int mode);

#endif

