#ifndef _GRIDSAUTH_H
#define _GRIDSAUTH_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDauth
//
//  Description: Authentication protocol for GRID nodes using TUNkey
//
//  Authentication Flow (with ephemeral keys):
//  1. After WebSocket handshake, server generates ephemeral keypair
//  2. Server sends AUTH_CHALLENGE with ephemeral public key + signature from static key
//  3. Client verifies signature, generates own ephemeral keypair
//  4. Client responds with AUTH_RESPONSE (ephemeral public key + signature from static key + encrypted challenge)
//  5. Server verifies client signature and challenge
//  6. Server sends AUTH_SUCCESS to confirm authentication
//  7. Both sides derive shared secret from ephemeral keys (forward secrecy)
//
//  Frame Formats:
//
//  AUTH_CHALLENGE Frame:
//  ┌───────────────────────────────────────────────────────────┐
//  │ COLuint16  version                                         │
//  │ uint8_t   msg_type  (GRIDWS_MSG_AUTH_CHALLENGE)           │
//  │ COLuint16  static_public_key_len                           │
//  │ uint8_t[] static_public_key_pem (static_public_key_len)   │
//  │ COLuint16  ephemeral_public_key_len                        │
//  │ uint8_t[] ephemeral_public_key_pem (ephemeral_public_key) │
//  │ COLuint16  challenge_len                                   │
//  │ uint8_t[] encrypted_challenge (challenge_len bytes)       │
//  │ COLuint16  signature_len                                   │
//  │ uint8_t[] signature (signature_len bytes)                 │
//  └───────────────────────────────────────────────────────────┘
//  Followed by: uint32_t CRC32
//
//  AUTH_RESPONSE Frame:
//  ┌────────────────────────────────────────────────────────────┐
//  │ COLuint16  version                                          │
//  │ uint8_t   msg_type  (GRIDWS_MSG_AUTH_RESPONSE)             │
//  │ COLuint16  static_public_key_len                            │
//  │ uint8_t[] static_public_key_pem (static_public_key_len)    │
//  │ COLuint16  ephemeral_public_key_len                         │
//  │ uint8_t[] ephemeral_public_key_pem (ephemeral_public_key)  │
//  │ COLuint16  response_len                                     │
//  │ uint8_t[] decrypted_challenge (response_len bytes)         │
//  │ COLuint16  signature_len                                    │
//  │ uint8_t[] signature (signature_len bytes)                  │
//  └────────────────────────────────────────────────────────────┘
//  Followed by: uint32_t CRC32
//
//  AUTH_SUCCESS Frame:
//  ┌──────────────────────────────────────────────┐
//  │ COLuint16  version                            │
//  │ uint8_t   msg_type  (GRIDWS_MSG_AUTH_SUCCESS)│
//  └──────────────────────────────────────────────┘
//  Followed by: uint32_t CRC32
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLstring.h>

class TUNkey;

struct GRIDauthChallengeFrame {
   COLstring StaticPublicKeyPem;
   COLstring EphemeralPublicKeyPem;
   COLstring EncryptedChallenge;
   COLstring Signature;
};

struct GRIDauthResponseFrame {
   COLstring StaticPublicKeyPem;
   COLstring EphemeralPublicKeyPem;
   COLstring DecryptedChallenge;
   COLstring Signature;
};

COLstring GRIDSbuildChallengeFrame(const TUNkey& MyStaticPrivateKey, const TUNkey& MyEphemeralPrivateKey);
COLstring GRIDSbuildResponseFrame(const TUNkey& MyStaticPrivateKey, const TUNkey& MyEphemeralPrivateKey,
                                     const GRIDauthChallengeFrame& Challenge, const TUNkey& ServerEphemeralPublicKey);
COLstring GRIDSbuildSuccessFrame(const COLstring& SessionKey);

bool GRIDSparseChallengeFrame(const COLstring& Frame, GRIDauthChallengeFrame* pOut);
bool GRIDSparseResponseFrame(const COLstring& Frame, const COLstring& ServerEphemeralKeyPem, GRIDauthResponseFrame* pOut);
bool GRIDSverifyResponse(const TUNkey& MyEphemeralPrivateKey, const GRIDauthResponseFrame& Response,
                            const COLstring& OriginalChallenge, const TUNkey& ClientEphemeralPublicKey,
                            const COLstring& ServerEphemeralKeyPem);
bool GRIDSverifySuccessFrame(const COLstring& Frame, const COLstring& SessionKey);

#endif
