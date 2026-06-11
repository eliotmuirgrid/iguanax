#ifndef __AVT_MESSAGES_H__
#define __AVT_MESSAGES_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: AVTmessages
//
// Description:
//
// Functions to standaridze output to terminal
//
// Author: Nicolas Lehman
// Date:   Sat 7 Dec 2019
//--------------------------------------------------------------------------

class COLostream;
class COLstring;

enum AVTmessageLevel {
   AVTwarn = 0,
   AVTerror,
   AVTcritical,
   AVTinfo,
   AVTfail,
   AVTpass,
};

void AVTmessage(COLostream& Stream, const COLstring& Message, AVTmessageLevel ErrorLevel);


#endif // End of defensive include