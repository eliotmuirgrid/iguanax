#ifndef __TEST_API_H__
#define __TEST_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testApi
//
// Description:
//
// Author: John Q
// Date:   Mon  4 Apr 2022 11:30:30 EDT
//---------------------------------------------------------------------------

class UNITapp;
class COLstring;

void testApi(UNITapp* pApp);
void logToMemory(const COLstring& Message);

#endif // end of defensive include
