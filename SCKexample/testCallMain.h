#ifndef __SCK_EXAMPLE_TEST_CALL_MAIN_H__
#define __SCK_EXAMPLE_TEST_CALL_MAIN_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCallMain 
//
// Description:
//
// Unit test for SCKcall - calls to main thread and blocks until it receives a response.
//
// Author: Vismay Shah
// Date:   Thu 30 Mar 2023 1:32pm
//---------------------------------------------------------------------------

class UNITapp;

void testCallMain(UNITapp* pApp);

#endif // end defensive include