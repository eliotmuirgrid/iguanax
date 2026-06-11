#ifndef __INS_PACKAGE_H__ // begin defensive include
#define __INS_PACKAGE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSpackage
//
// Description:
//
// Installer packaging utility functions.
// 
// Author: John Qi
// Date:   Mon 24 Jun 2019 10:07:16 EDT
//
//---------------------------------------------------------------------------

class COLstring;

// appends the Payload file to the Transport File
void INSpack(const COLstring& Transport, const COLstring& Payload, bool isDirectory = false);

// extracts the payload from the Carrier file and save it as Output file
void INSunpack(const COLstring& Carrier, const COLstring& Output);

#endif // end defensive include
