// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTwaitForEnterWindows
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski  
// Date:   Thursday 15 June 2023 - 11:05AM
// ---------------------------------------------------------------------------


#include <COL/COLwindows.h>

#include <conio.h>
#include <time.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void INSTwaitForEnter(int Seconds){
   COL_FUNCTION(INSTwaitForEnter);
   time_t End = time(NULL) + Seconds;
   while(time(NULL) < End){
      if (_kbhit()){
         char Char = _getch();
	 if (Char == '\r' || Char == '\n') {
            COL_TRC("Got an enter.");
            break;
	 }
      }
   } 
}
