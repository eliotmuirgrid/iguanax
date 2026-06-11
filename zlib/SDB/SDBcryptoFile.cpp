// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcryptoFile
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Mon Oct 28 14:14:58 EDT 2024
// ---------------------------------------------------------------------------
#include <SDB/SDBcrypto.h>
#include <SDB/SDBcryptoFile.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBreader.h>
#include <SDB/SDBstring.h>
#include <SDB/SDBenvVar.h>

#include <FIL/FILosDefs.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


int SDBencMarkerSize() {
   return strlen(SDB_ENCRYPTED_ENVELOPE_MARKER);
}

static int SDBencSize(const COLstring& AAD, const COLstring& IV, const COLstring& Ciphertext, const COLstring& Tag) {
   COLASSERT(!AAD.is_null());    // if AAD is an empty string, SDBstringSize(AAD) returns 1
   return SDBstringSize(AAD) + SDBstringSize(IV) + SDBstringSize(Ciphertext) + SDBstringSize(Tag);
}

// static void SDBencryptedEnvelopeWrite(COLstring* pBuffer, const COLstring& IV, const COLstring& Ciphertext, const COLstring& Tag) {
//    COL_FUNCTION(SDBencryptedEnvelopeWrite);
//    COL_VAR3(IV.size(), Ciphertext.size(), Tag.size());
//    pBuffer->append(SDB_ENCRYPTED_ENVELOPE_MARKER);
//    int PayloadSize = SDBcrcSize() + Content.size();
//    COL_VAR(PayloadSize);
//    COLstring Number;
//    int AmountWritten = SDBwriteNumber(PayloadSize, Number.get_buffer());
//    Number.setSize(AmountWritten);
//    pBuffer->append(Number);
//    COLuint32 Checksum = SFIcrc::calculate(Content.data(), Content.size());
//    COL_VAR(Checksum);
//    pBuffer->append((char *)&Checksum, sizeof(COLuint32));
//    pBuffer->append(Content);
// }

// serialize the IV, Ciphertext, and Tag of a AES-GCM encryption to buffer
// Format : Marker + total size + IV + Ciphertext + Tag
// Decided to not fix the IV and Tag size in case the size recommendations change in the future.
int SDBwriteEncryptedData(const COLstring& AAD, const COLstring& IV, const COLstring& Ciphertext, const COLstring& Tag, COLstring* pBuffer){
   COL_FUNCTION(SDBwriteEncryptedData);
   COLstring& Buffer = *pBuffer;
   int PayloadSize = SDBencSize(AAD, IV, Ciphertext, Tag);
   int PacketSize  = SDBencMarkerSize() + SDBnumberSize(PayloadSize) + PayloadSize;
   Buffer = SDB_ENCRYPTED_ENVELOPE_MARKER;         // marker
   COL_DUMP("Marker", Buffer.data(), PacketSize + 1);
   Buffer.setCapacity(PacketSize);
   char* pOut = Buffer.get_buffer() + SDBencMarkerSize();
   pOut += SDBwriteNumber(PayloadSize, pOut);      // total size of IV, ciphertext, and tag
   COL_DUMP("Size", Buffer.data(), PacketSize + 1);
   pOut += SDBwriteString(AAD,          pOut);     // AAD
   COL_DUMP("AAD", Buffer.data(), PacketSize + 1);
   pOut += SDBwriteString(IV,          pOut);      // IV
   COL_DUMP("IV", Buffer.data(), PacketSize + 1);
   pOut += SDBwriteString(Ciphertext,  pOut);      // ciphertext
   COL_DUMP("Ciphertext", Buffer.data(), PacketSize + 1);
   pOut += SDBwriteString(Tag,         pOut);      // tag
   COL_DUMP("Tag", Buffer.data(), PacketSize + 1);
   int Amount = pOut - Buffer.get_buffer();
   COLASSERT(Amount == PacketSize);
   Buffer.setSize(PacketSize);
   return Amount;
}

COLstring SDBgenerateAAD(int Version) {
   COL_FUNCTION(SDBgenerateAAD);
   // we only put the version number in AAD for now
   // serializing a number will never take more than 16B
   COLstring AAD;
   int AADSize = SDBwriteNumber(1, AAD.get_buffer());    // log version 1
   AAD.setSize(AADSize);
   return AAD;
}

// Data is serialized plaintext message(s)
// TODO: what if Data is 10 100MB messages enqueued at the same time?
int SDBencryptAndSerialize(const COLstring& Data, const COLstring& EncryptionKey, COLstring* pBuffer) {
   COL_FUNCTION(SDBencryptAndSerialize);
   COLstring AAD = SDBgenerateAAD(1);
   COLstring IV, Ciphertext, Tag;
   SDBencrypt(Data, EncryptionKey, IV, &Ciphertext, &Tag, AAD);
   int Size = SDBwriteEncryptedData(AAD, IV, Ciphertext, Tag, pBuffer);
   return Size;
}

static int SDBreadEncryptedData2(const char* pBuffer, int BufSize, COLstring* pAAD, COLstring* pIV, COLstring* pCiphertext, COLstring* pTag) {
   COL_FUNCTION(SDBreadEncryptedData2);
   if (BufSize < SDBencMarkerSize() + 4) {   // we need at least the marker and total size in the buffer
      COL_TRC("Not enough data in buffer");
      return -1;
   }
   if (strncmp(pBuffer, SDB_ENCRYPTED_ENVELOPE_MARKER, SDBencMarkerSize()) != 0) {
      COL_TRC("Encrypted data should begin with a marker.");
      return -4;
   }
   const char* pIn = pBuffer + SDBencMarkerSize();
   BufSize -= SDBencMarkerSize();
   COLuint64 PayloadSize = 0;
   int Amount = 0;
   do {
      Amount = SDBreadNumber(&PayloadSize,   pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      // TODO: what if payload is 10 100MB messages enqueued at the same time?
      if (PayloadSize < 0 || PayloadSize > SDBmaxMessageSizeMB() * SDB_SIZE_MB * 2) { COL_TRC("corrupted msg"); return -5; }
      Amount = SDBreadString(pAAD,           pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadString(pIV,            pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadString(pCiphertext,    pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      Amount = SDBreadString(pTag,           pIn, BufSize); if (Amount < 0) { break; } pIn += Amount; BufSize -= Amount;
      return pIn - pBuffer;
   } while (false);
   if (Amount < -1) {
      COL_ERR("Failed to read number: " << Amount);
   } else {
      COL_ERR("Not enough data for a encryption packet");
   }
   return Amount;
}

int SDBparseAndDecrypt(const char* pBuffer, int BufSize, const COLstring& EncryptionKey, COLstring* pPlaintext) {
   COL_FUNCTION(SDBparseAndDecrypt);
   COLstring AAD, IV, Ciphertext, Tag, Plaintext;
   int Amount = SDBreadEncryptedData2(pBuffer, BufSize, &AAD, &IV, &Ciphertext, &Tag);
   if (Amount < 0) {
      COL_ERR("Failed to parse encryption packet because of error " << Amount);
      return Amount;
   }
   COLuint64 Version;
   SDBreadNumber(&Version, AAD.data(), AAD.size());
   COL_VAR(Version);
   if (!SDBdecrypt(Ciphertext, Tag, IV, EncryptionKey, &Plaintext, AAD)) {
      COL_ERR("Failed to decrypt encrypted data");
      return -5;
   }
   pPlaintext->append(Plaintext);
   return Amount;
}

int SDBfindNextEncMarker(const char* pBuffer, int BufSize) {
   COL_FUNCTION(SDBfindNextEncMarker);
   // start the search after marker size in case buffer begins with marker
   const char* pPos = strstr(pBuffer + SDBencMarkerSize(), SDB_ENCRYPTED_ENVELOPE_MARKER);
   if (pPos == NULL) { return -1; }
   return pPos - pBuffer;
}
