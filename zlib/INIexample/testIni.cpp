//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testIni
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Fri 24 Mar 2023 12:20:28 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <FIL/FILutils.h>

#include <INI/INIdata.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testSsh() {
	COL_FUNCTION(testSsh);
	INIdata Config;
	INIdataKeySet(&Config, "ssh", "core", "sshCommand", "ssh -o \"StrictHostKeyChecking=no\" -o \"IdentitiesOnly=yes\" -i C:/Program Files/Iguana Test/configurations/users/admin/privatekey");
	COLstring Content;
	INIformat(Config, &Content);
	const COLstring Expected = "[ssh \"core\"]\n\tsshCommand = ssh -o \"StrictHostKeyChecking=no\" -o \"IdentitiesOnly=yes\" -i \"C:/Program Files/Iguana Test/configurations/users/admin/privatekey\"\n";
	UNIT_ASSERT_EQUALS(Expected, Content);
}

static void testFormat(){
   COL_FUNCTION(testFormat);
   INIdata Config;
   INIdataKeySet(&Config, "branch", "master", "remote", "origin");
   INIdataKeySet(&Config, "branch", "master", "merge", "refs/heads/master");

   COLstring Content;
   INIformat(Config, &Content);

   INIdata Out;
   INIparse(Content, &Out);
   COLstring Content2;
   INIformat(Out, &Content2);
   COL_DUMP("Original", Content.c_str(),  Content.size());
   COL_DUMP("Copy",     Content2.c_str(), Content2.size());
   UNIT_ASSERT_EQUALS(Content2, Content);
}

void testIni(UNITapp* pApp) {
   pApp->add("ini/format", &testFormat);
	pApp->add("ini/ssh", &testSsh);

}
