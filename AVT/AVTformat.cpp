//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: AVTformat
//
// Description:
//
// Functions to colorize terminal output and detect if color is supported
//
// Author: Nicolas Lehman
// Date:   Tue 19 Nov 2019
//--------------------------------------------------------------------------

#include "AVTformat.h"
#include "AVTextFormat.h"

#include <COL/COLlog.h>
#include <COL/COLostream.h>
#include <COL/COLsinkString.h>
#include <COL/COLstring.h>
COL_LOG_MODULE;

#include <PRO/PROexecute.h>

#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

static bool ColorOn = false;

bool AVTareColorsEnabled() {
   return ColorOn;
}

void AVTdisableColors() {
   ColorOn = false;
}

bool AVTenableColors() {
   COL_FUNCTION(AVTenableColors);
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

int AVTnumColorsSupported() {
   COL_FUNCTION(AVTnumColorsSupported);
#ifdef _WIN32
   return 8;
#endif
   COLstring ExitString;
   int ExitCode = PROexecuteString(".", "tput colors", &ExitString, &ExitString);
   if (0 == ExitCode) {
      return atoi(ExitString.c_str());
   } else {
      COL_VAR2(ExitCode, ExitString);
      return -1;
   }
}

COLostream& operator<<(COLostream& Stream, const AVTformat& Format){
   if (ColorOn) {
      Stream << Format.Format();
   }
   return Stream;
}

COLostream& operator<<(COLostream& Stream, const AVTextFormat& ExtFormat) {
   if (ColorOn && AVTnumColorsSupported() > 16) {
      Stream << ExtFormat.Format();
   }
   return Stream;
}