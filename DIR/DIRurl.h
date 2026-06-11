#ifndef __DIR_URL_H__
#define __DIR_URL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DIRurl
//
// Description:
//
// Base URL for downloads - shifted here for few dependencies.
//
// Author: Eliot Muir
// Date:   Sunday 13 August 2023 - 10:43AM
// ---------------------------------------------------------------------------

class COLstring;

COLstring DIRurlDownloads();
COLstring DIRurlWindowsDownloads();
COLstring DIRreleaseServer();
COLstring DIRinstallServer();
COLstring DIRcrashServer();

#endif // end of defensive include
