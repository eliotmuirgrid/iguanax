//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXfilterEncrypt
//
// Description:
//
// Implementation.
//
// Author: Henry Tran
// Date: July 6, 2004
//
//
//---------------------------------------------------------------------------


#include "DRXfilterEncrypt.h"
#include <SFI/SFIaesEncryptor.h>
#include <SFI/SFImd5.h>
// #include <COL/COLsimpleBuffer.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


class DRXfilterEncryptPrivate
{
public:
   DRXfilterEncryptPrivate()
      : pEncryptor( NULL ),
        AmountInBuffer( 0 ),
        SizeOfEncryptionKey( 0 ),
        KeyWritten( false ) {}
   ~DRXfilterEncryptPrivate();

   SFIaesEncryptor* pEncryptor;

   COLuint8 EncryptionBuffer[ DRX_AES_SIZE_OF_BLOCK ];
   COLuint8 AmountInBuffer;

   COLuint8 EncryptionKey[ DRX_AES_SIZE_OF_KEY ];
   COLuint8 SizeOfEncryptionKey;
   bool KeyWritten;
};

DRXfilterEncryptPrivate::~DRXfilterEncryptPrivate()
{
   if ( pEncryptor != NULL )
   {
      delete pEncryptor;
   }
}

DRXfilterEncrypt::DRXfilterEncrypt( COLsink* pNext_ )
   : COLfilter( pNext_ )
{
   pMember = new DRXfilterEncryptPrivate();
}

DRXfilterEncrypt::~DRXfilterEncrypt()
{
   delete pMember;
}

void DRXfilterEncrypt::setEncryptionKey( const COLuint8* pEncryptionKey, COLuint8 SizeOfEncryptionKey )
{
   if ( SizeOfEncryptionKey > DRX_AES_SIZE_OF_KEY )
   {
      COL_ERROR_STREAM_PLAIN( "Encryption key is larger than maximum size of "
                              << DRX_AES_SIZE_OF_KEY << " bytes.", COLerror::PreCondition );
   }

   ::memset( pMember->EncryptionKey, '\0', DRX_AES_SIZE_OF_KEY );
   ::memcpy( pMember->EncryptionKey, pEncryptionKey, SizeOfEncryptionKey );

   pMember->SizeOfEncryptionKey = SizeOfEncryptionKey;

   if ( pMember->pEncryptor != NULL )
   {
      delete pMember->pEncryptor;
   }

   pMember->pEncryptor = new SFIaesEncryptor( pMember->EncryptionKey );
}

const COLuint8* DRXfilterEncrypt::encryptionKey() const
{
   COLPRECONDITION( pMember->pEncryptor != NULL )
   return pMember->EncryptionKey;
}

COLuint8 DRXfilterEncrypt::sizeOfEncryptionKey() const
{
   COLPRECONDITION( pMember->pEncryptor != NULL )
   return pMember->SizeOfEncryptionKey;
}

COLuint32 DRXfilterEncrypt::write( const void* pBuffer, COLuint32 SizeOfBuffer ){
   COL_METHOD(DRXfilterEncrypt::write);
   COL_VAR(COLstring((const char*)pBuffer, SizeOfBuffer));

   const COLuint32 OriginalSizeOfBuffer = SizeOfBuffer;

   if ( pMember->pEncryptor == NULL ){
      COL_ERROR_STREAM( "Need to set encryption key for AES encryptor.", COLerror::PreCondition );
   }

   if ( !pMember->KeyWritten ){
      COL_TRC("Key not written.");
      SFImd5Hasher Hasher;
      Hasher.write( pMember->EncryptionKey, DRX_AES_SIZE_OF_KEY );
      next().write( Hasher.hash().data(), DRX_AES_SIZE_OF_BLOCK );
      pMember->KeyWritten = true;
      COL_TRC("Key written.");
   }

   COLuint8 AmountToWrite;
   while (SizeOfBuffer > 0){
      COLuint32 MaxSizeOfBlock = DRX_AES_SIZE_OF_BLOCK - 1; // First byte of block stores block size
      COL_VAR2(SizeOfBuffer, MaxSizeOfBlock);

      if (SizeOfBuffer > (MaxSizeOfBlock - pMember->AmountInBuffer)){
         AmountToWrite = MaxSizeOfBlock - pMember->AmountInBuffer;
      }else{
         AmountToWrite = SizeOfBuffer;
      }
      unsigned char* pWriteTo = pMember->EncryptionBuffer + 1 + pMember->AmountInBuffer;
      COL_VAR3(pWriteTo, pBuffer, (int)AmountToWrite);
      COL_DUMP("Block:", pBuffer, AmountToWrite);
      ::memcpy(pWriteTo, pBuffer, AmountToWrite);
      pMember->AmountInBuffer += AmountToWrite;

      if (pMember->AmountInBuffer == MaxSizeOfBlock){
         pMember->EncryptionBuffer[0] = MaxSizeOfBlock;
         COL_DUMP("Unencrypted BLock:", pMember->EncryptionBuffer, DRX_AES_SIZE_OF_BLOCK);
         pMember->pEncryptor->ProcessBlock(pMember->EncryptionBuffer, pMember->EncryptionBuffer);
         COL_DUMP("Encrypted BLock:", pMember->EncryptionBuffer, DRX_AES_SIZE_OF_BLOCK);
         next().write(pMember->EncryptionBuffer, DRX_AES_SIZE_OF_BLOCK);
         pMember->AmountInBuffer = 0;
      }

      if (SizeOfBuffer > AmountToWrite){
         pBuffer = (COLuint8*)pBuffer + AmountToWrite;
      }
      SizeOfBuffer -= AmountToWrite;
      COL_VAR(SizeOfBuffer);
      
   }
   
   return OriginalSizeOfBuffer;
}

void DRXfilterEncrypt::flush(){
   COL_METHOD(DRXfilterEncrypt::flush);
   if ( pMember->pEncryptor == NULL ){
      COL_ERROR_STREAM( "Need to set encryption key for AES encryptor.", COLerror::PreCondition );
   }

   // Last block always contains the size of any overflow block, including 0 size.
   COLuint8 PaddedEncryptionBuffer[DRX_AES_SIZE_OF_BLOCK];
   ::memset( PaddedEncryptionBuffer, '\0', DRX_AES_SIZE_OF_BLOCK );
   
   ::memcpy( PaddedEncryptionBuffer + 1, pMember->EncryptionBuffer + 1, pMember->AmountInBuffer );
   PaddedEncryptionBuffer[0] = pMember->AmountInBuffer;
   
   pMember->pEncryptor->ProcessBlock( PaddedEncryptionBuffer, PaddedEncryptionBuffer );
   next().write( PaddedEncryptionBuffer, DRX_AES_SIZE_OF_BLOCK );
   pMember->AmountInBuffer = 0;

   next().flush();
}

void DRXfilterEncrypt::onEndStream()
{
   next().onEndStream(); 
   pMember->KeyWritten = false;
}
