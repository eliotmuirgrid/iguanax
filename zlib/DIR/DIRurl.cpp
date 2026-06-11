// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DIRurl
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sunday 13 August 2023 - 10:43AM
// ---------------------------------------------------------------------------

#include <DIR/DIRurl.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

const char* DIR_WINDOWS_URL = "https://winbuild.interfaceware.org:7777/";
const char* DIR_RELEASE_URL = "https://downloads.interfaceware.com/";
static const COLstring DIR_INSTALL_URL = "https://install.interfaceware.com:8080/";
static const COLstring DIR_URL_CRASH   = "https://crashreport.interfaceware.com:7888/";

COLstring DIRurlDownloads(){
#ifdef _WIN32
   return DIR_WINDOWS_URL;
#elif __APPLE__
   return "https://macbuild.interfaceware.org:7777/";
#else
#if defined(__arm__) || defined(__aarch64__)
   return "https://linuxarmbuild.interfaceware.org:7777/";
#else
   return "https://linuxbuild.interfaceware.org:7777/";
#endif
#endif
}

COLstring DIRurlWindowsDownloads(){
   COL_FUNCTION(DIRurlWindowsDownloads);
   return DIR_WINDOWS_URL;
}

COLstring DIRreleaseServer() {
   COL_FUNCTION(DIRreleaseServer);
   return DIR_RELEASE_URL;
}

COLstring DIRinstallServer() {
   return DIR_INSTALL_URL;
}

COLstring DIRcrashServer() {
   return DIR_URL_CRASH;
}
