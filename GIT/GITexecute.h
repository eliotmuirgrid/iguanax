#ifndef __GIT_EXECUTE_H__
#define __GIT_EXECUTE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITexecute
//
// Description:
//
// A wrapper around PROexecute that has retry logic to deal with git functions 
// that make use of the .git/index.lock file.
// 
// Git functions that make use of the index.lock file : add, reset, clean, 
// commit, and pull
//
// Author: Tyler Brown 
// Date:   Tuesday 07 November 2023 - 11:02AM
// ---------------------------------------------------------------------------

class COLstring;

int GITexecuteString(const COLstring& Dir, const COLstring& Command, COLstring* pOut, COLstring* pErr);


#endif // end of defensive include
