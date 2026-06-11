#ifndef __INS_ASCII_H__
#define __INS_ASCII_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSascii
//
// Description:
//
// Wrappers for ascii color support.  We detect and switch it on if available
// in Windows - but degrade to black and white if ascii is not available.
//    
// Author: Tyler Brown and Eliot Muir
// Date:   Mon 11 Nov 2019 10:41:02 EST
//---------------------------------------------------------------------------

class COLostream;
class COLstring;

// Needed for windows to enable color codes
// Will set a static variable.
bool INSenableColorCodes();

void INSclearScreen();
COLostream& bright_green(COLostream& _outs);
COLostream& black(COLostream& _outs);
COLostream& reset(COLostream& _outs);
COLostream& green_back(COLostream& _outs);
int INSerror(const COLstring& Error);


#endif // end defensive include
