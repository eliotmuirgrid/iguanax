#ifndef __PRO_CHILD_H__
#define __PRO_CHILD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROchild
//
// Description:
//
// Code for executing child - it it's own CPP for easier tracing.
//
// Author: Eliot Muir 
// Date:   Monday 16 January 2023 - 05:23PM
// ---------------------------------------------------------------------------

class COLstring;

void PROexecChild(const COLstring& Dir, const COLstring& Command,  int StandardOut[2], int StandardErr[2], int CommPipe[2]);

#endif // end of defensive include
