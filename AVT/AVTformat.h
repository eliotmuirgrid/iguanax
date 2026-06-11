#ifndef __AVT_FORMAT_H__
#define __AVT_FORMAT_H__
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
//---------------------------------------------------------------------------

class COLostream;
#include <COL/COLstring.h>

enum AVTcolors {
   AVTblack = 30,
   AVTred,
   AVTgreen,
   AVTyellow,
   AVTblue,
   AVTmagenta,
   AVTcyan,
   AVTwhite
};

enum AVTbgColors {
   AVTbgBlack = 40,
   AVTbgRed,
   AVTbgGreen,
   AVTbgYellow,
   AVTbgBlue,
   AVTbgMagenta,
   AVTbgCyan,
   AVTbgWhite
};

enum AVTtextDecoration {
   AVTbold       = 1,
   AVTunderline  = 4,
   AVTinvert     = 7,
   AVTreset      = 0
};

int  AVTnumColorsSupported();

bool AVTareColorsEnabled();
bool AVTenableColors();
void AVTdisableColors();

class AVTformat{
public:
   AVTformat(int Color) : m_Color(Color) {}

   COLstring Format() const { return "\033[" + COLintToString(m_Color) + "m"; }
private:
   int m_Color;
};

COLostream& operator<<(COLostream& Stream, const AVTformat& Format);

#endif // end defensive include
