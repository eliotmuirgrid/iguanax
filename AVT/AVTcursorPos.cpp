//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: AVTcursorPos
//
// Description:
//
// Functions to move the cursor on the terminal
//
// Author: Nicolas Lehman
// Date:   Tue 3 Dec 2019
//--------------------------------------------------------------------------

#include "AVTcursorPos.h"
#include "AVTclear.h"

#include <COL/COLlog.h>
#include <COL/COLstring.h>
#include <COL/COLostream.h>
COL_LOG_MODULE;


COLstring AVTcursorPos::AVTgetDirection(AVTdirection Direction) const {
   switch (Direction)
   {
   case AVTup:
      return "A";
      break;
      case AVTdown:
      return "B";
      break;
   case AVTright:
      return "C";
      break;
   case AVTleft:
      return "D";
      break;
   default:
      break;
   }
}

COLostream& operator<<(COLostream& Stream, const AVTcursorPos& CursorPos) {
   Stream << CursorPos.move();
   return Stream;
}
