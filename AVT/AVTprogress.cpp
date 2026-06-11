//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: AVTprogress
//
// Description:
//
// Functions to create progress bars in the terminal
//
// Author: Nicolas Lehman
// Date:   Tue 3 Dec 2019
//--------------------------------------------------------------------------

#include "AVTclear.h"
#include "AVTcursorPos.h"
#include "AVTformat.h"
#include "AVTprogress.h"

#include <COL/COLstring.h>
#include <COL/COLostream.h>

static int mCount = 0;

void AVTdotLoad (int CountMax, int CountMod) {
   if (mCount >= 0 && mCount <= CountMax){
      if (0 == mCount % CountMod) {
         COLcout << "." << flush;
      }
         mCount++;
   } else {
      mCount = 0;
      COLcout << AVTclear(AVTclearLine) << AVTcursorPos(1000, AVTleft);
   }
}