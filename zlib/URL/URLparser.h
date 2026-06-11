#ifndef __URL_PARSER_H__
#define __URL_PARSER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: URLparser
//
// Description:
//
// URLparser - routine to parse a URL.  This library dated from 2009 and was never
// used.  I rewrote in March 24 2023  
//
// Author: Eliot Muir
// Date:   Tuesday, May 5th, 2009 @ 09:44:28 AM
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLstring;

enum URLtype{
   URL_HTTP   =0,
   URL_FTP    =1,
   URL_FILE   =2,
   URL_SSH    =3,
   URL_UNKNOWN=4
};

URLtype URLurlType(const COLstring& Url);

bool URLparseHttpUrl(const COLstring& UrlIn, bool* pIsHTTPS, COLstring* pHost, COLstring* pPath, int* pPort);
bool URLparseFileUrl(const COLstring& Url, COLstring* pPath);

// URL formats
// ssh://git@bitbucket.org/interfaceware/hl7-viewer.git
// git@bitbucket.org:interfaceware/hl7-viewer.git  
// https://bitbucket.org/interfaceware/hl7-viewer.git

#endif // end of defensive include