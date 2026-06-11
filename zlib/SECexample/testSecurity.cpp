
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testDirectoryAccess.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   20/12/24 10:31 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <SEC/SECpathParser.h>
#include <UNIT/UNITapp.h>

#include "testSecurity.h"
COL_LOG_MODULE;

static void assertCommand(const COLstring& Command, const int Expected) {
   UNIT_ASSERT_EQUALS(Expected, SECpathParse(Command).size());
}

static void testPathParser() {
   COL_FUNCTION(testPathParser);
   assertCommand("ls /Users/test/Library/", 1);
   assertCommand("curl --version", 0);
   assertCommand(R"("C:\Program Files\IguanaX\iguana.exe" --reset_admin_user)", 1);
   assertCommand(R"("/iguana.exe" --reset_admin_user)", 1);
   assertCommand(R"(randomFileDownloadProgram "/downloads/badprogram.exe" /home/notAgoodPlace)", 2);
   assertCommand(R"(netstat -an | grep” .. Configs.Port)", 0);

}

void testDirectoryAccess(UNITapp* pApp) {
   COL_FUNCTION(testPathParser);
   pApp->add("path/simple", &testPathParser);
}