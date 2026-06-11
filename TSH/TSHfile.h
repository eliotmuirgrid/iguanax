#ifndef __TSH_FILE_H__
#define __TSH_FILE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSHfile
//
// Description:
//
// TSHfile class.
//
// Author: Dmitri Klimenko
// Date:   Friday, July 27th, 2007 @ 12:18:11 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include <COL/COLstring.h>

bool TSHfileCutExtension(const COLstring& Source, const char* Ext, COLstring& Result);

COLstring TSHfileFixNewLines(const char* NewLine, const COLstring& Source);


COLstring TSHfileRead( const COLstring& FileName );

void TSHfileWrite( const COLstring& FileName, const COLstring& Data);

#endif // end of defensive include
