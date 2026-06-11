#ifndef __SDB_CRYPTO_FILE_H__
#define __SDB_CRYPTO_FILE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcryptoFile
//
// Description:
//
// Serialization functions for encrypted logs
//
// Author: John Q
// Date:   Mon Oct 28 14:14:58 EDT 2024
// ---------------------------------------------------------------------------
class COLstring;

#define SDB_ENCRYPTED_ENVELOPE_MARKER "(:"   // don't put null byte in the marker or SDBencMarkerSize() returns wrong value

int SDBencMarkerSize();

int SDBencryptAndSerialize(const COLstring& Data, const COLstring& EncryptionKey, COLstring* pBuffer);
int SDBparseAndDecrypt(const char* pBuffer, int BufSize, const COLstring& EncryptionKey, COLstring* pPlaintext);

int SDBfindNextEncMarker(const char* pBuffer, int BufSize);

#endif // end of defensive include
