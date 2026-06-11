//------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  PROkill
// 
// Description
//
// kill a process
// 
// Author: Shuo Hou
// Date:   June 5 2021
//---------------------------------------------------------------------------
#include "PROkill.h"

#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <COL/COLerror.h>
#include <COL/COLstring.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void PROkill(int PID){
   COL_FUNCTION(PROkill);
   int retVal = kill(PID, SIGKILL);
   if(retVal == 0){
      COL_TRC("Process has been killed");
   } else {
      switch(errno)
      {
         case EINVAL:
            COL_TRC("An invalid signal was specified\n");
         break;
         case EPERM:
            COL_TRC("Current process does not have permission to send the signal to process");
         break;
         case ESRCH:
            COL_TRC("Process does not exist");
         break;
         default:
            COL_TRC("Other error with code");
            COL_VAR(errno);
         break;
      }
   }
}