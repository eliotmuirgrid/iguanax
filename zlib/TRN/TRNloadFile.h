#ifndef __TRN_LOAD_FILE_H__
#define __TRN_LOAD_FILE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNloadFile
//
// Description:
//
// TRNloadFile - routines to load files.
//
// Author: Eliot Muir
// Date:   Friday, November 26th, 2010 @ 09:58:21 PM
//---------------------------------------------------------------------------

class COLstring;
// Load code from a file (into pCode).
// Returns true if load successful, false if file doesn't exist.
bool TRNloadFile(const COLstring& FullFilePath, COLstring* pCode);

#endif // end of defensive include
