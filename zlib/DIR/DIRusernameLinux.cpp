// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DIRusernameLinux
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Monday 28 August 2023 - 01:27PM
// ---------------------------------------------------------------------------

#include <DIR/DIRusernameLinux.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifndef _WIN32
#  include <unistd.h>
#  include <stdlib.h>
#endif

COLstring DIRusernameLinux(){
   COL_FUNCTION(DIRusernameLinux);
    COLstring Username;
#ifdef __linux__ 
   uid_t uid       = getuid();
   char *user      = getenv("USER");
   char *sudo_uid  = getenv("SUDO_UID");
   char *sudo_user = getenv("SUDO_USER");
   if (uid == 0 && sudo_uid == 0) {
      COL_TRC("We are running as root");
      Username = "root";
   }
   else{
      if (sudo_uid) {
         COL_TRC("Running with sudo as user (UID:" << sudo_uid << ")");
         Username = sudo_user;
      } else{
         COL_TRC("Running as a regular user");
         Username = user;
      }
   }
#endif
   return Username;
}