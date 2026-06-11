#ifndef __TUN_HASH_H__
#define __TUN_HASH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNhash
//
// Description:
//
// We need to express the public keys in a format that works for COLhashmap
//
// Author: Eliot Muir 
// Date:   Saturday 24 June 2023 - 11:59AM
// ---------------------------------------------------------------------------

class COLstring;
class TUNkey;

COLstring TUNhashKey(const COLstring& Der);
COLstring TUNhashKey(const TUNkey& Key);

#endif // end of defensive include
