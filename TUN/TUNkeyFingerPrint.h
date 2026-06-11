#ifndef __TUN_KEY_FINGER_PRINT_H__
#define __TUN_KEY_FINGER_PRINT_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNkeyFingerPrint
//
// Description:
//
// Produce a finger print of a key.
//
// Author: Eliot Muir 
// Date:   Saturday 24 June 2023 - 10:51AM
// ---------------------------------------------------------------------------

class COLstring;
class TUNkey;

COLstring TUNkeyFingerPrintFromDer(const COLstring& Der);
COLstring TUNkeyFingerPrint(const TUNkey& Key);

#endif // end of defensive include
