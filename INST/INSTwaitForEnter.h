#ifndef __INST_WAIT_FOR_ENTER_H__
#define __INST_WAIT_FOR_ENTER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTwaitForEnter
//
// Description:
//
// Routine to wait a number of seconds or when the user presses enter.
// Useful for environments like Windows which close terminal windows quickly
// when a program exits.
//
// Author: Eliot Muir
// Date:   Wed 08/09/2023 
// ---------------------------------------------------------------------------

void INSTwaitForEnter(int Seconds);

#endif // end of defensive include
