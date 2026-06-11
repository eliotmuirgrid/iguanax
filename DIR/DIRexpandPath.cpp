// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DIRexpandPath
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Monday 28 August 2023 - 12:07PM
// ---------------------------------------------------------------------------

#include <DIR/DIRexpandPath.h>
#include <DIR/DIRusernameLinux.h>

#include <FIL/FILpathUtils.h>

#include <COL/COLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifndef _WIN32
#  include <unistd.h>
#  include <pwd.h>
#endif

COLstring DIRexpandPath(const COLstring& OriginalPath){
   COL_FUNCTION(DIRexpandPath);
   if(OriginalPath.is_null()){
      return OriginalPath;
   }
#ifndef __linux__
   return FILexpand(OriginalPath);
#else
   COLstring Path = COLexpandEnvironmentVariables(OriginalPath);
   if (Path == "~" || (Path.size() >= 2 && Path[0] == '~' && (Path[1] == '/' || Path[1] == '\\'))){
      COLstring User = DIRusernameLinux();
      COLstring Home;
      if (User.is_null()){
         struct passwd *pw = ::getpwuid(::getuid());
         Home = pw->pw_dir;
      }
      else if(User == "root"){
         Home = "/root/";
      }
      else{
         Home = "/home/" + User;
      }
      if (!Home.is_null()){
         return Home + Path.substr(1);
      }
   }
   return Path;
#endif
}