#ifndef __WEB_FILE_HANDLER_H__
#define __WEB_FILE_HANDLER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBfileHandler
//
// Author: John Qi
//
// Description:
//
// A file request handler for WEBserver.
//---------------------------------------------------------------------------
class COLvar;
class SCKloop;
class WEBrequestData;

#include <COL/COLclosure.h>
#include <COL/COLstring.h>

// To make this work with indirection we need to be able to give it a path and get a callback on COLclosure1<const COLstring&> on the
// main thread which will allow us to direct the data where we need to.

void WEBfileResponse(const WEBrequestData& Data, SCKloop* pLoop, const COLstring& Index = "index.html");

void WEBfileResponseReal(const COLstring& Path, COLclosure1<const COLstring&>* pCallback, SCKloop* pLoop, bool Compressed = true, const COLstring& Index = "index.html");

// for unit test
// EncodingList is the value of the 'Accept-Encoding' header
// Format is a compression algo. e.g. gzip, deflate
bool WEBhasCompressionFormat(COLstring EncodingList, COLstring Format);

#endif // end of defensive include
