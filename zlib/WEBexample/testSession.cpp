// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSession
//
// Description:
//
// Implementation of tests
//
// Author: Eliot Muir
// Date:   Saturday 20 May 2023 - 09:34AM
// ---------------------------------------------------------------------------
#include "testSession.h"

#include <UNIT/UNITapp.h>

#include <WEB/WEBsession.h>
#include <WEB/WEBsessionHandler.h>
#include <WEB/WEBrequestData.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void WEBTpermissionCheck(COLstring Username, COLvar* pPermissions, bool* pIsAdmin) {
   COL_VAR(Username);
   *pIsAdmin = "foo" == Username;
}

static void testSession(){
   COL_FUNCTION(testSession);
   COLmap<COLstring, WEBsession> SessionMap;
   SessionMap["SessionIdOne"].m_Username = "foo";
   SessionMap["SessionIdTwo"].m_Username = "bar";

   WEBpermissionsSet(COLnewClosure3(&WEBTpermissionCheck));

   WEBrequestData Request;
   Request.IsSecure = false;
   Request.Headers["cookie"] = "IguanaSession=SessionIdOne";

   COLstring User;
   COLvar Permissions;
   bool Admin = false;
   UNIT_ASSERT(WEBwho(SessionMap, Request, &User, &Admin, &Permissions));
   UNIT_ASSERT(Admin);
   UNIT_ASSERT_EQUALS("foo", User);

   Request.Headers["cookie"] = "IguanaSession=SessionIdTwo";
   UNIT_ASSERT(WEBwho(SessionMap, Request, &User, &Admin, &Permissions));
   UNIT_ASSERT(!Admin);
   UNIT_ASSERT_EQUALS("bar", User);
}


void testSession(UNITapp* pApp) {
   pApp->add("session/user", &testSession);
}
