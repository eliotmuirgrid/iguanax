//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: USRgenerateAdminIfEmpty.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   02/11/23 3:59 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GUID/GUIDid.h>
#include <USR/USRgenerateAdminIfEmpty.h>
#include <USR/USRmap.h>
#include <USR/USRsetAdmin.h>
COL_LOG_MODULE;

#ifndef _WIN32
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#endif

void USRchown(const COLstring& Path, const COLstring& UserName){
   COL_FUNCTION(USRchown);
   COL_VAR2(Path, UserName);
#ifndef _WIN32
   struct passwd* pw = getpwnam(UserName.c_str());
   if (NULL == pw) {
      COLcout << "Failed to change user ownership of " << Path << " to " << UserName << ". " << COLerrorString(errno) << newline;
      return;
   }
   struct group* gr = getgrnam(UserName.c_str());
   if (NULL == gr) {
      COLcout << "Failed to change group ownership of " << Path << " to " << UserName << ". " << COLerrorString(errno) << newline;
      return;
   }
   if (chown(Path.c_str(), pw->pw_uid, gr->gr_gid) == -1) {
      COLcout << "Failed to change ownership of " << Path << " to " << UserName << ". " << COLerrorString(errno) << newline;
   }
#endif
}


void USRgenerateAdminIfEmpty(const COLstring& UserName) {
   COL_FUNCTION(USRgenerateAdminIfEmpty);
   COLmap<COLstring, USRuser> UserMap;
   USRload(&UserMap);
   if(UserMap.empty()) {
      COL_TRC("No users found -- setting new admin password");
      COLstring Password = GUIDnewId(16);
      USRsetAdminPassword(Password);
      USRchown(DIRconfigFile("users/admin/user.config"), UserName);
      USRchown(DIRconfigFile("users/admin"), UserName);
      USRchown(DIRconfigFile("users/"), UserName);
      USRchown(DIRconfigFile(""), UserName);
      USRchown(FILworkingDir(), UserName);
      COLcout << "Login with:" << newline
              << "   User:     admin" << newline
              << "   Password: " << Password << newline << newline;
   }
}