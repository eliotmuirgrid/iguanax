#ifndef __INS_MAKE_EXEC_H__ // begin defensive include
#define __INS_MAKE_EXEC_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSmakeExec
//
// Description:
//
// Routine to loop through directory and make the files executable.
// 
// Author: Eliot Muir
// Date:   Sat  6 Jul 2019 10:27:21 EDT
//---------------------------------------------------------------------------

class COLstring;

// Does what it says on the above description
void INSmakeExec(const COLstring& Dir);

#endif // end defensive include
