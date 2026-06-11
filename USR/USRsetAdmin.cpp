// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USRsetAdmin
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Friday 18 August 2023 - 01:28PM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <POP/POPcolor.h>
#include <USR/USRpermissions.h>
#include <USR/USRsetAdmin.h>
COL_LOG_MODULE;

void USRsaveAdmin(const COLmap<COLstring, USRuser>& UserMap){
   COL_FUNCTION(USRsaveAdmin);
   COLvar Content = UserMap["admin"].toVar(true);
   USRmodifyVarForSave(Content);
   COLstring JsonOut = Content.json(1);
   COLstring FileName = DIRconfigUserDir(UserMap["admin"].username()) + "user.config";
   FILcreateDirectoryTreeToFile(FileName, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   FILwriteFile(FileName, JsonOut);
}

void USRsetAdminPassword(const COLstring& Password){
   COL_FUNCTION(USRsetAdminPassword);
   COLmap<COLstring, USRuser> UserMap;
   USRload(&UserMap);
   USRaddAdminUser(&UserMap, Password);
   USRsaveAdmin(UserMap);
}

void USRadminPasswordDocumentation(const COLstring& Password, COLostream& OutStream){
   COL_FUNCTION(USRadminPasswordDocumentation);
   COLcout << "Updated login credentials to:" << newline 
           << "   User:     admin" << newline
           << "   Password: " << Password << newline << newline;
   if (Password == "password"){
      OutStream << POPwarning("Oh please - really?") << " I hope your boss isn't looking - you picked 'password' as your" << newline;
      OutStream << "password. You will need to fix this before Iguana will run. Use this command:" << newline;    
      OutStream << POPiguanaCommandLine() << " --set_admin_password <something secure!>" << newline;
      return;
   }
   OutStream << "To apply these changes you will need to restart IguanaX, use:" << newline; 
   #ifdef __APPLE__
   OutStream << "  " << POPiguanaCommandLine() << " --service install" << newline;
   #else
   OutStream << "  " << POPiguanaCommandLine(true) << " --service restart" << newline;
   #endif
}

void USRsetAdmin(const COLstring& Password, COLostream& OutStream){
   COL_FUNCTION(USRsetAdmin);
   USRsetAdminPassword(Password);
   USRadminPasswordDocumentation(Password, OutStream);
}
