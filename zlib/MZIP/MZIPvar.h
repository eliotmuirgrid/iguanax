#ifndef __MZIP_VAR_H__
#define __MZIP_VAR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: MZIPvar
//
// Description:
//
// MZIPvar - routines to extract zip data in memory into a COLvar
//
// Author: Eliot Muir
// Date:   Thu 13 Mar 2014 14:52:18 EDT
//---------------------------------------------------------------------------

class COLostream;
class COLstring;
class COLvar;

extern "C" {
   struct unz_file_info_s;
   typedef struct unz_file_info_s unz_file_info;
}

enum MZIPresult {
   MZIP_SUCCESS = 0,
   MZIP_CORRUPT = 1,
   MZIP_FAILURE = 2
};

// Unzip an in-memory zip file to a COLvar.
// Returns MZIP_SUCCESS on success.
// *pError will be populated with error text upon failure.
int MZIPunzipToVar(const void* pData, int Size, const char* Password, bool WantMetaData, COLvar* pVar, COLstring* pError);

// Produce an in-memory zip "file" from a COLvar tree.
// Returns MZIP_SUCCESS on success.
// *pError will be populated with error text upon failure.
int MZIPzipVar(const COLvar& InMemoryFileTree, COLstring* pMemoryZip, COLstring* pErr=0);

COLostream& operator<<(COLostream& Stream, const unz_file_info& FileInfo);

#endif // end of defensive include

