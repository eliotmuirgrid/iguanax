//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXfilterDecrypt
//
// Description:
//
// Implementation.
//
// Author: Henry Tran
// Date: July 6, 2004
//
//---------------------------------------------------------------------------

#include "DRXfilterDecrypt.h"

#include <SFI/SFIaesDecryptor.h>
#include <SFI/SFImd5.h>
// #include <COL/COLsimpleBuffer.h>

class DRXfilterDecryptPrivate
{
public:
   DRXfilterDecryptPrivate()
      : pDecryptor( NULL ),
        AmountInBuffer( 0 ),
        SizeOfDecryptionKey( 0 ),
        KeyVerified( false ) {}
   ~DRXfilterDecryptPrivate();

   SFIaesDecryptor* pDecryptor;

   COLuint8 DecryptionBuffer[ DRX_AES_SIZE_OF_BLOCK ];
   COLuint8 AmountInBuffer;

   COLuint8 DecryptionKey[ DRX_AES_SIZE_OF_KEY ];
   COLuint8 SizeOfDecryptionKey;
   bool KeyVerified;
};

DRXfilterDecryptPrivate::~DRXfilterDecryptPrivate()
{
   if ( pDecryptor != NULL )
   {
      delete pDecryptor;
   }
}

DRXfilterDecrypt::DRXfilterDecrypt( COLsink* pNext_ )
   : COLfilter( pNext_ )
{
   pMember = new DRXfilterDecryptPrivate();
}

DRXfilterDecrypt::~DRXfilterDecrypt()
{
   delete pMember;
}

void DRXfilterDecrypt::setDecryptionKey( const COLuint8* pDecryptionKey, COLuint8 SizeOfDecryptionKey )
{
   if ( SizeOfDecryptionKey > DRX_AES_SIZE_OF_KEY )
   {
      COL_ERROR_STREAM_PLAIN( "Decryption key is larger than maximum size of "
                              << DRX_AES_SIZE_OF_KEY << " bytes.", DRX_KEY_TOO_LARGE );
   }

   ::memset( pMember->DecryptionKey, '\0', DRX_AES_SIZE_OF_KEY );
   ::memcpy( pMember->DecryptionKey, pDecryptionKey, SizeOfDecryptionKey );

   pMember->SizeOfDecryptionKey = SizeOfDecryptionKey;

   if ( pMember->pDecryptor != NULL )
   {
      delete pMember->pDecryptor;
   }

   pMember->pDecryptor = new SFIaesDecryptor( pMember->DecryptionKey );
}

const COLuint8* DRXfilterDecrypt::decryptionKey() const
{
   COLPRECONDITION( pMember->pDecryptor != NULL )
   return pMember->DecryptionKey;
}

COLuint8 DRXfilterDecrypt::sizeOfDecryptionKey() const
{
   COLPRECONDITION( pMember->pDecryptor != NULL )
   return pMember->SizeOfDecryptionKey;
}

COLuint32 DRXfilterDecrypt::write( const void* pBuffer, COLuint32 SizeOfBuffer )
{
   const COLuint32 OriginalSizeOfBuffer = SizeOfBuffer;

   if ( pMember->pDecryptor == NULL )
   {
      COL_ERROR_STREAM( "Need to set decryption key for AES decryptor.", DRX_KEY_NO_DECRYPTION_KEY );
   }

   COLuint8 AmountToWrite;
   while ( SizeOfBuffer > 0 )
   {
      if ( SizeOfBuffer > (DRX_AES_SIZE_OF_BLOCK - pMember->AmountInBuffer) )
      {
         AmountToWrite = DRX_AES_SIZE_OF_BLOCK - pMember->AmountInBuffer;
      }
      else
      {
         AmountToWrite = SizeOfBuffer;
      }

      ::memcpy( pMember->DecryptionBuffer + pMember->AmountInBuffer, pBuffer, AmountToWrite );
      pMember->AmountInBuffer += AmountToWrite;

      if( pMember->AmountInBuffer == DRX_AES_SIZE_OF_BLOCK )
      {
         if ( !pMember->KeyVerified )
         {
            SFImd5Hasher Hasher;
            Hasher.write( pMember->DecryptionKey, DRX_AES_SIZE_OF_KEY );

            if ( ::memcmp( Hasher.hash().data(), pMember->DecryptionBuffer, DRX_AES_SIZE_OF_BLOCK ) != 0 )
            {
               // #8844
               // This is changed to reflect something more user friendly. 
               // because this is the message that gets bubbled up to users
               // of Iguana with a license key with different Machine ID
               COL_ERROR_STREAM( "Encryption key does not match decryption key.", DRX_KEY_MISMATCH);
            }

            pMember->KeyVerified = true;
         }
         else
         {
            pMember->pDecryptor->ProcessBlock( pMember->DecryptionBuffer, pMember->DecryptionBuffer );
            COLuint8 SizeOfBlock = pMember->DecryptionBuffer[0];
            next().write( pMember->DecryptionBuffer + 1, SizeOfBlock );
         }
         pMember->AmountInBuffer = 0;
      }

      if ( SizeOfBuffer > AmountToWrite )
      {
         pBuffer = (COLuint8*) pBuffer + AmountToWrite;
      }
      SizeOfBuffer -= AmountToWrite;
   }

   return OriginalSizeOfBuffer;
}

void DRXfilterDecrypt::onEndStream()
{
   next().onEndStream(); 
   pMember->KeyVerified = false;
};
