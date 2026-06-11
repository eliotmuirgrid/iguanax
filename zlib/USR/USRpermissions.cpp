//   ---------------------------------------------------------------------------
//   Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
//   Module: USRpermissions
//
//   Description: Implementation
//
//   Author: Matthew Sobkowski
//   Date:  9/21/21, 1:18 PM
//   ---------------------------------------------------------------------------
#include "USRpermissions.h"
#include "USRsetAdmin.h"

#include <POP/POPcolor.h>

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <DIR/DIRpath.h>
#include <FIL/FILatomic.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
COL_LOG_MODULE;

bool USRmapContainsAdmin(COLmap<COLstring, USRuser>* pUserMap){
   COL_FUNCTION(USRmapContainsAdmin);
   for(const auto& i : *pUserMap) {
      COL_VAR(i->first);
      if(i->first == "admin") { return true; }
   }
   return false;
}

void USRaddAdminUser(COLmap<COLstring, USRuser>* pUserMap, const COLstring& Password){
   COL_FUNCTION(USRaddAdminUser);
   COL_VAR(Password);
   if(pUserMap->count("admin") > 0) {
      COLarray<COLstring> Tags = (*pUserMap)["admin"].roles();
      (*pUserMap)["admin"].setRoles(Tags);
      (*pUserMap)["admin"].setPassword(Password); // we might or might not be overriding the password
   } else {
      USRuser User;
      COLarray<COLstring> Tags;
      User.setUsername("admin");
      User.setRoles(Tags);
      User.setPassword(Password);
      pUserMap->add(User.username(), User);
   }
}

bool USRcheckNeedsAdminUser(COLostream& Out, bool Verbose){
   COL_FUNCTION(USRcheckNeedsAdminUser);
   COLmap<COLstring, USRuser> UserMap;
   USRload(&UserMap);
   bool ContainsAdmin = USRmapContainsAdmin(&UserMap);
   COL_VAR(ContainsAdmin);
   if(!ContainsAdmin){
      Out << "To login and create users you'll need to run this command to create an admin account." << newline; 
      Out << "Respect the security of your organization by picking a secure password:" << newline;
      Out << "   " << POPiguanaCommandLine(false) << " --set_admin_password <pick secure password here>" << newline;
      return true;
   }
   return false;
}

void USRensureAdminUserExists(){
   COL_FUNCTION(USRensureAdminUserExists);
   COLmap<COLstring, USRuser> UserMap;
   USRload(&UserMap);
   bool AdminExists = USRmapContainsAdmin(&UserMap);
   if(AdminExists){
      COL_TRC("Admin user exists, no need to modify the user map.");
      return;
   }
   COL_TRC("Map does not contain admin user: adding default admin user");
   USRaddAdminUser(&UserMap, "password");
   USRsaveAdmin(UserMap);
}
