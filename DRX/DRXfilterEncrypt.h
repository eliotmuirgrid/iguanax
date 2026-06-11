#ifndef __DRX_FILTER_ENCRYPT_H__
#define __DRX_FILTER_ENCRYPT_H__

//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2005 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXfilterEncrypt
//
// Description:
//
// Copied from STMCRYPTO2/DRXfilterEncrypt.
//
// This class performs AES encryption of a data stream.
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
//---------------------------------------------------------------------------

#include <COL/COLfilter.h>

#include "DRXconstant.h"

class DRXfilterEncryptPrivate;

class DRXfilterEncrypt : public COLfilter
{
public:
   DRXfilterEncrypt( COLsink* pNext_ );
   virtual ~DRXfilterEncrypt();

   // Maximum key size is 32 bytes. Any unused bytes are 0-padded to produce
   // a fixed-length key of 32 bytes.
   void setEncryptionKey( const COLuint8* pEncryptionKey, COLuint8 SizeOfEncryptionKey );
   const COLuint8* encryptionKey() const;
   COLuint8 sizeOfEncryptionKey() const;

   virtual COLuint32 write( const void* cpBuffer, COLuint32 SizeOfBuffer );
   virtual void flush();
   virtual void onEndStream();
   
   virtual void resetFilter(){};

private:
   DRXfilterEncryptPrivate* pMember;
   
   DRXfilterEncrypt( DRXfilterEncrypt& Orig ); // not allowed
   DRXfilterEncrypt& operator=( DRXfilterEncrypt& Orig ); // not allowed
};

#endif // end of defensive include
