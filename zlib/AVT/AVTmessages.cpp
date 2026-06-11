//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: AVTcolorize
//
// Description:
//
// Functions to colorize terminal output and detect if color is supported
//
// Author: Nicolas Lehman
// Date:   Tue 19 Nov 2019
//--------------------------------------------------------------------------

#include "AVTmessages.h"
#include "AVTformat.h"

#include <COL/COLlog.h>
#include <COL/COLostream.h>
#include <COL/COLstring.h>
COL_LOG_MODULE;

void AVTmessage(COLostream& Stream, const COLstring& Message, AVTmessageLevel MessageLevel) {
   COL_FUNCTION(AVTmessage);
   COLstring Level;
   AVTbgColors BgColor;
   AVTcolors Color;
   COL_VAR(MessageLevel);
   switch (MessageLevel)
   {
   case AVTwarn:
      Level = "WARNING:";
      Color = AVTyellow;
      BgColor = AVTbgYellow;
      break;
   case AVTerror:
      Level = "ERROR:";
      Color = AVTred;
      BgColor = AVTbgRed;
      break;
   case AVTcritical:
      Level = "CRITICAL:";
      Color = AVTred;
      BgColor = AVTbgRed;
      break;
   case AVTinfo:
      Level = "INFO:";
      Color = AVTwhite;
      BgColor = AVTbgWhite;   
      break;
   case AVTfail:
      Level = "FAIL:";
      Color = AVTred;
      BgColor = AVTbgRed;   
      break;
   case AVTpass:
      Level = "PASS:";
      Color = AVTgreen;
      BgColor = AVTbgGreen;
      break;
   default:
      Level = "UNKNOWN MESSAGE LEVEL:";
      Color = AVTwhite;
      BgColor = AVTbgWhite;
      break;
   }

   Stream << AVTformat(BgColor) << AVTformat(AVTblack);
   Stream << Level << AVTformat(AVTreset) << " " << AVTformat(Color) << Message;
   Stream << AVTformat(AVTreset) << newline;
}
