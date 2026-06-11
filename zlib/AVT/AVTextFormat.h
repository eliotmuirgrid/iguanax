#ifndef __AVT_EXT_FORMAT_H__
#define __AVT_EXT_FORMAT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: AVTformat
//
// Description:
//
// Class to support 256 color terminals
//
// Author: Nicolas Lehman
// Date:   Sat 7 Dec 2019
//---------------------------------------------------------------------------

class COLstring;

class AVTextFormat{
public:
   AVTextFormat(COLstring ExtendedColor) : m_ExtendedColor(ExtendedColor) {}

   COLstring Format() const { return "\033[" + m_ExtendedColor + "m"; }
private:
   COLstring m_ExtendedColor;
};

COLostream& operator<<(COLostream& Stream, const AVTextFormat& Format);

#endif // end defensive include