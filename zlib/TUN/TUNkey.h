#ifndef __TUN_KEY_H__
#define __TUN_KEY_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNkey
//
// Description:
//
// Helpers to handle memory management with openssl.
//
// Author: Eliot Muir 
// Date:   Tuesday 13 June 2023 - 10:30AM
// ---------------------------------------------------------------------------

struct evp_pkey_st;
struct evp_pkey_ctx_st;

class TUNkey{
public:
   TUNkey(){}
   ~TUNkey();
   evp_pkey_st* key = nullptr;
   // TODO - more work required before I can make these private
//private:
//   TUNkey(const TUNkey& Key);
//   TUNkey& operator=(const TUNkey& Key);
};

class TUNkeyContext{
public:
   TUNkeyContext() {}
   ~TUNkeyContext();
   evp_pkey_ctx_st* pContext = nullptr;
private:
   TUNkeyContext(const TUNkeyContext& Key);
   TUNkeyContext& operator=(const TUNkeyContext& Key);
};

#endif // end of defensive include
