//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSascii
//
// Description:
//
// See header file.
//
// Author: Tyler Brown and Eliot Muir
// Date:   Mon 11 Nov 2019 10:43:42 EST
//---------------------------------------------------------------------------

#include "INSascii.h"
#include <stdlib.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef _WIN32
#  include <windows.h>
#endif

static bool ColorOn = false;

// Needed for windows to enable color codes
// Will set a static variable.
bool INSenableColorCodes(){
   COL_FUNCTION(INSenableColorCodes);
#ifdef _WIN32
    // Set output mode to handle virtual terminal sequences
   HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
   if (hOut == INVALID_HANDLE_VALUE){
     COL_TRC("Unable to get standard handle.");
     return false;
   }
   DWORD dwMode = 0;
   if (!GetConsoleMode(hOut, &dwMode)){
      COL_TRC("Unable to get console mode.");
      return false;
   }
   dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
   if (!SetConsoleMode(hOut, dwMode)){
      COL_TRC("Ascii graphics not supported.");
      return false;
   }
#endif
   COL_TRC("Ascii graphics supported.");
   ColorOn = true;
   return true;
}

void INSclearScreen(){
   COL_FUNCTION(INSclearScreen);
   if (ColorOn){
      COLcout << "\033[2J\033[H";  // clear screen and go home
   }
}

COLostream& bright_green(COLostream& _outs){
   if (ColorOn){
      _outs << "\033[32;1m";
   }
   return _outs;
}

COLostream& black(COLostream& _outs){
   if (ColorOn){
      _outs << "\033[30;1m";
   }
   return _outs;
}

COLostream& reset(COLostream& _outs){
   if(ColorOn){
      _outs << "\033[0m";
   }
   return _outs;
}

COLostream& green_back(COLostream& _outs){
   if (ColorOn){
      _outs << "\033[42m";
   }
   return _outs;
}

int INSerror(const COLstring& Error){
   if (ColorOn){
      COLcout << "\033[41;1m\033[37m" << Error << "\033[0m" << newline;
   }
   else {
      COLcout << Error << newline;
   }
   return EXIT_FAILURE;
}
