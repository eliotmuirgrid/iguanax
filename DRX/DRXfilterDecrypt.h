#ifndef __DRX_FILTER_DECRYPT_H__
#define __DRX_FILTER_DECRYPT_H__

//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXfilterDecrypt
//
// Description:
//
// Copied from STMCRYPTO2/DRXfilterDecrypt.
//
// This class performs AES decryption of a data stream.
//
// Fixed-length 32 byte keys are used in this class. Any unused bytes are
// 0-padded. This also means that keys cannot be longer than 32 bytes.
// (AES can actually use keys of length 16, 24, or 32 bytes. For the 
// greatest flexibility, 32 byte keys were chosen).
//
// An MD5 hash of the encryption key is placed at the beginning of the 
// encryption stream in order to verify the decryption when decrypting
// this stream.
//
// Additionally, AES works by encrypting fixed-size blocks of 16 bytes.
// To handle data streams that are not an exact multiple of 16 bytes in 
// size, the actual size of each block is stored in the block's first byte.
//
// Author: Henry Tran
// Date: July 6, 2004 
//
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <COL/COLfilter.h>

#include "DRXconstant.h"

class DRXfilterDecryptPrivate;

enum DRXfilterDecryptErrorCode
{
   DRX_KEY_TOO_LARGE = 100,
   DRX_KEY_NO_DECRYPTION_KEY = 101,
   DRX_KEY_MISMATCH = 102,
   DRX_GENERAL_ERROR = 103
};

class COL_DLL DRXfilterDecrypt : 
   public COLfilter
{
public:
   DRXfilterDecrypt( COLsink* pNext_ );
   virtual ~DRXfilterDecrypt();

   // Maximum key size is 32 bytes. Any unused bytes are 0-padded to produce
   // a fixed-length key of 32 bytes.
   void setDecryptionKey( const COLuint8* pDecryptionKey, COLuint8 SizeOfDecryptionKey );
   const COLuint8* decryptionKey() const;
   COLuint8 sizeOfDecryptionKey() const;

   //error code in exceptions can be DRXfilterDecryptErrorCode
   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );
   virtual void onEndStream();
   
   virtual void resetFilter(){};

private:
   DRXfilterDecryptPrivate* pMember;
   
   DRXfilterDecrypt( DRXfilterDecrypt& Orig ); // not allowed
   DRXfilterDecrypt& operator=( DRXfilterDecrypt& Orig ); // not allowed
};

#endif // end of defensive include
