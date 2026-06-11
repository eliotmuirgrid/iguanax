#ifndef __AVT_CURSOR_POS_H__
#define __AVT_CURSOR_POS_H__
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

class COLostream;
#include <COL/COLstring.h>

enum AVTdirection {
   AVTup = 0,
   AVTdown,
   AVTright,
   AVTleft
};

class AVTcursorPos{
public:
   AVTcursorPos(int CharactersToMove, AVTdirection Direction) : m_CharactersToMove(CharactersToMove), m_Direction(Direction) {}

   COLstring AVTgetDirection(AVTdirection Direction) const;
   COLstring move() const { return "\033[" + COLintToString(m_CharactersToMove) + AVTgetDirection(m_Direction).c_str(); }

   int CharacterstToMove() {return m_CharactersToMove; }
   int Direction() {return m_Direction; }
private:
   int m_CharactersToMove;
   AVTdirection m_Direction;
};

COLostream& operator<<(COLostream& Stream, const AVTcursorPos& CursorPos);

#endif // end of defensive include.