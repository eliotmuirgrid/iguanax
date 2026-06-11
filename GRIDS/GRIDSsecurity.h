#ifndef _GRIDSSECURITY_H
#define _GRIDSSECURITY_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDsecurity
//
//  Description: Security layer for GRID using TUNkey cryptography
// The protocol provides:
//   Authenticated DH with forward secrecy
//   Identity binding to ephemeral keys
//   Session authentication for all messages
//   Proper key derivation following cryptographic best practices
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLstring.h>

class TUNkey;

void      GRIDSloadOrGenerateKey(TUNkey* pKey);
COLstring GRIDSencrypt(const TUNkey& MyPrivateKey, const TUNkey& PeerPublicKey, const COLstring& Plaintext);
COLstring GRIDSdecrypt(const TUNkey& MyPrivateKey, const TUNkey& PeerPublicKey, const COLstring& Ciphertext);
COLstring GRIDSgetPublicKeyPem(const TUNkey& Key);
void      GRIDSloadPublicKeyPem(const COLstring& PemString, TUNkey* pKey);
COLstring GRIDSsign(const TUNkey& PrivateKey, const COLstring& Data);
bool      GRIDSverifySignature(const TUNkey& PublicKey, const COLstring& Data, const COLstring& Signature);
COLstring GRIDSderiveKey(const COLstring& SharedSecret, const COLstring& Salt, const COLstring& Info);
COLstring GRIDScomputeHMAC(const COLstring& Key, const COLstring& Data);

#endif
