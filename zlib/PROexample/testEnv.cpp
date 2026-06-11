//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testEnv
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:    Thu 23 Mar 2023 11:30:52 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <PRO/PROenv.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testSimple(){
   COL_FUNCTION(testSimple);
   COLvar Env;
   PROfullEnv(&Env);
   COL_VAR(Env.json(1));
   // Case sensitivity for Path/PATH on windows means we are probably doing to need to do extra work.
#ifdef _WIN32
   UNIT_ASSERT_EQUALS(Env.exists("Path"), true); // WTF - why does windows not make this uppercase?
#else
   UNIT_ASSERT_EQUALS(Env.exists("PATH"), true);
#endif
}

void testEnv(UNITapp* pApp) {
   pApp->add("env/cat",        &testSimple);
}
